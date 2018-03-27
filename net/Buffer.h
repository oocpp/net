//
// Created by lg on 17-4-19.
//
//
// Created by lg on 17-5-3.
//

#pragma once
#include<vector>
#include <algorithm>
#include <cassert>
#include <utility>

namespace net{
    class Buffer final
    {
    private:
        void ensure_writable_bytes(size_t len) {

            if (get_writable_size() < len) {
                if (get_writable_size()+_read_index < len) {
                    _buff.resize(_write_index+len);
                }
                else{
                    std::copy(_buff.begin()+_read_index,_buff.begin()+_write_index,_buff.begin());
                    _write_index-=_read_index;
                    _read_index=0;
                }
            }
        }

    public:

        Buffer()
                :_write_index(0)
                ,_read_index(0){
        }

        std::pair<ssize_t,int> read_from_fd(int fd);

        size_t get_writable_size() const{
            return _buff.size()-_write_index;
        }

        size_t length() const {
            assert(_write_index >= _read_index);
            return _write_index - _read_index;
        }


        void append(const char*data,size_t len){
            assert(data!= nullptr);

            ensure_writable_bytes(len);

            std::copy_n(data,len,_buff.begin()+_write_index);
            has_write(len);
        }

        void append(std::initializer_list<std::pair<const char*,std::size_t>>args){
            std::size_t len=0;
            for(auto &t:args){
                assert(t.first!= nullptr);
                len+=t.second;
            }

            ensure_writable_bytes(len);

            for(auto &t:args) {
                std::copy_n(t.first, t.second, _buff.begin()+_write_index);
                has_write(t.second);
            }
        }


        size_t get_readable_size() const{
            return _write_index - _read_index;
        }

        char* get_read_ptr(){
            return _buff.data()+_read_index;
        }

        char* get_write_ptr(){
            return _buff.data()+_write_index;
        }

        void has_read(size_t n){
            _read_index+=n;
            assert(_read_index<=_write_index);
        }

        void has_write(size_t n){
            _write_index+=n;
            assert(_write_index<=_buff.size());
        }

    private:
        std::vector<char>_buff;
        size_t _read_index;
        size_t _write_index;
    };

}
