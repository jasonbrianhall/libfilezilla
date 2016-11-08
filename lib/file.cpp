#include "libfilezilla/libfilezilla.hpp"
#include "libfilezilla/file.hpp"

#ifndef FZ_WINDOWS
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace fz {

file::file()
{
}

file::file(native_string const& f, mode m, creation_flags d)
{
	open(f, m, d);
}

file::~file()
{
	close();
}

#ifdef FZ_WINDOWS
bool file::open(native_string const& f, mode m, creation_flags d)
{
	close();

	DWORD dispositionFlags;
	if (m == writing) {
		if (d == empty) {
			dispositionFlags = CREATE_ALWAYS;
		}
		else {
			dispositionFlags = OPEN_ALWAYS;
		}
	}
	else {
		dispositionFlags = OPEN_EXISTING;
	}

	DWORD shareMode = FILE_SHARE_READ;
	if (m == reading) {
		shareMode |= FILE_SHARE_WRITE;
	}

	hFile_ = CreateFile(f.c_str(), (m == reading) ? GENERIC_READ : GENERIC_WRITE, shareMode, 0, dispositionFlags, FILE_FLAG_SEQUENTIAL_SCAN, 0);

	return hFile_ != INVALID_HANDLE_VALUE;
}

void file::close()
{
	if (hFile_ != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile_);
		hFile_ = INVALID_HANDLE_VALUE;
	}
}

int64_t file::size() const
{
	int64_t ret = -1;

	LARGE_INTEGER size{};
	if (GetFileSizeEx(hFile_, &size)) {
		ret = static_cast<int64_t>(size.QuadPart);
	}
	return ret;
}

int64_t file::seek(int64_t offset, seek_mode m)
{
	int64_t ret = -1;

	LARGE_INTEGER dist{};
	dist.QuadPart = offset;

	DWORD method = FILE_BEGIN;
	if (m == current) {
		method = FILE_CURRENT;
	}
	else if (m == end) {
		method = FILE_END;
	}

	LARGE_INTEGER newPos{};
	if (SetFilePointerEx(hFile_, dist, &newPos, method)) {
		ret = newPos.QuadPart;
	}
	return ret;
}

bool file::truncate()
{
	return !!SetEndOfFile(hFile_);
}

int64_t file::read(void *buf, int64_t count)
{
	int64_t ret = -1;

	DWORD read = 0;
	if (ReadFile(hFile_, buf, static_cast<DWORD>(count), &read, 0)) {
		ret = static_cast<int64_t>(read);
	}

	return ret;
}

int64_t file::write(void const* buf, int64_t count)
{
	int64_t ret = -1;

	DWORD written = 0;
	if (WriteFile(hFile_, buf, static_cast<DWORD>(count), &written, 0)) {
		ret = static_cast<int64_t>(written);
	}

	return ret;
}

bool file::opened() const
{
	return hFile_ != INVALID_HANDLE_VALUE;
}

bool remove_file(fz::native_string const& name)
{
	return DeleteFileW(name.c_str()) != 0;
}

#else

bool file::open(native_string const& f, mode m, creation_flags d)
{
	close();

	int flags = O_CLOEXEC;
	if (m == reading) {
		flags |= O_RDONLY;
	}
	else {
		flags |= O_WRONLY | O_CREAT;
		if (d == empty) {
			flags |= O_TRUNC;
		}
	}
	fd_ = ::open(f.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

#if HAVE_POSIX_FADVISE
	if (fd_ != -1) {
		(void)posix_fadvise(fd_, 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_NOREUSE);
	}
#endif

	return fd_ != -1;
}

void file::close()
{
	if (fd_ != -1) {
		::close(fd_);
		fd_ = -1;
	}
}

int64_t file::size() const
{
	int64_t ret = -1;

	struct stat buf;
	if (!fstat(fd_, &buf)) {
		ret = buf.st_size;
	}

	return ret;
}

int64_t file::seek(int64_t offset, seek_mode m)
{
	int64_t ret = -1;

	int whence = SEEK_SET;
	if (m == current) {
		whence = SEEK_CUR;
	}
	else if (m == end) {
		whence = SEEK_END;
	}

	auto newPos = lseek(fd_, offset, whence);
	if (newPos != static_cast<off_t>(-1)) {
		ret = newPos;
	}

	return ret;
}

bool file::truncate()
{
	bool ret = false;

	auto length = lseek(fd_, 0, SEEK_CUR);
	if (length != static_cast<off_t>(-1)) {
		do {
			ret = !ftruncate(fd_, length);
		} while (!ret && (errno == EAGAIN || errno == EINTR));
	}

	return ret;
}

int64_t file::read(void *buf, int64_t count)
{
	int64_t ret;
	do {
		ret = ::read(fd_, buf, count);
	} while (ret == -1 && (errno == EAGAIN || errno == EINTR));

	return ret;
}

int64_t file::write(void const* buf, int64_t count)
{
	int64_t ret;
	do {
		ret = ::write(fd_, buf, count);
	} while (ret == -1 && (errno == EAGAIN || errno == EINTR));

	return ret;
}

bool file::opened() const
{
	return fd_ != -1;
}

bool remove_file(fz::native_string const& name)
{
	return unlink(name.c_str()) == 0;
}

#endif

}
