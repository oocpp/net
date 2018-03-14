//
// Created by lg on 17-4-19.
//
//
// Created by lg on 17-5-3.
//

#pragma once
#include<vector>
#include<list>
#include <algorithm>
#include <cassert>
#include <utility>

namespace net{
    class Buffer final
    {
    private:
        size_t ensureWritableBytes(size_t len) {

            if (_buff.capacity() - _buff.size() < len) {
                if (_read_index > _buff.size() / 2) {
                    std::copy(_buff.begin() + _read_index, _buff.end(), _buff.begin());
                    _buff.resize(_buff.size() - _read_index);
                    _read_index = 0;
                }

                if (_buff.capacity() < _buff.max_size() / 2)
                    _buff.reserve(_buff.capacity() * 2);
                else
                    _buff.reserve(_buff.max_size());
            }
            size_t index=_buff.size();
            _buff.resize(index+len);

            assert(_buff.size()==index+len);

            return index;
        }

    public:

        Buffer()
                :_read_index(0)
                ,_headspace_index(0){
        }

        void append(const char*data,size_t len){
            assert(data!= nullptr);
            size_t index=ensureWritableBytes(len);
            std::copy_n(data,len,_buff.begin()+index);
        }

        void append(std::initializer_list<std::pair<const char*,std::size_t>>args){
            std::size_t len=0;
            for(auto &t:args){
                assert(t.first!= nullptr);
                len+=t.second;
            }

            len=ensureWritableBytes(len);

            for(auto &t:args) {
                std::copy_n(t.first, t.second, _buff.begin()+len);
                len+=t.second;
            }
        }

        void assign(std::vector<char>&& data) {
            if(_read_index==0) {
                _buff = std::move(data);
            }
            else{
                append(data.data(),data.size());
            }

        }

        void setHeadSpace(size_t n){
            _headspace_index=ensureWritableBytes(n);
        }

        void setHeadSpaceData(const char*data,size_t len){
            assert(data!=nullptr);

            std::copy_n(data,len,_buff.begin()+_headspace_index);
        }

        size_t getReadableSize() const{
            return _buff.size()-_read_index;
        }

        char* readPtr(){
            return _buff.data()+_read_index;
        }

        void hasRead(size_t n){
            _read_index+=n;
            assert(_read_index<_buff.size());
        }
    private:
        std::vector<char>_buff;
        size_t _headspace_index;
        size_t _read_index;
    };

}
