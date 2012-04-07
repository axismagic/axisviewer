#include "camera_viewer.h"
#include "stream_handler.h"
#include "ui/main_window.h"

#include <curl/curl.h>

#include <memory>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <vector>

camera_viewer::camera_viewer(const char * url, const char * delim) :
    stream_handle(trans, delim)
{
	curl_handle = std::unique_ptr<CURL, CURL_deleter>(curl_easy_init());
	curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url);
	//full debug
	curl_easy_setopt(curl_handle.get(), CURLOPT_VERBOSE, 0L);
	//disable progress
	curl_easy_setopt(curl_handle.get(), CURLOPT_NOPROGRESS, 1L);
	//set callback
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEFUNCTION, stream_handler::write);
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, (void*)&stream_handle);
	//create multi object for nonblocking input/output
	set = std::unique_ptr<CURL, CURLM_deleter>(curl_multi_init());
	if (!set) {
		throw std::runtime_error("curl_multi_init() failed");
	}
	curl_multi_add_handle(set.get(), curl_handle.get());
	remaining = 1;
}

camera_viewer::~camera_viewer() {
	curl_multi_remove_handle(set.get(), curl_handle.get());
}

int camera_viewer::receive() {
	if (remaining > 0) {
		curl_multi_perform(set.get(), &remaining);
	}
    else {
        trans.lost_comm();
    }
	return remaining;
}

jpeg2pixbuf::signal_lost_comm_t& camera_viewer::signal_lost_comm() {
    return trans.signal_lost_comm();
}

jpeg2pixbuf::signal_new_image_t& camera_viewer::signal_new_image() {
    return trans.signal_new_image();
}
