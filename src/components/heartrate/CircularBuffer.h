#pragma once

#include <array>

namespace Pinetime
{
   namespace Controllers
   {
       template <typename T, std::size_t N>
       class CircularBuffer {
           public:
                CircularBuffer(){};
                ~CircularBuffer() = default;

                bool IsFull() const {
                    return _full;
                }

                T Get(size_t index) const {
                    return buffer[index % buffer.size()];
                }

                void Append(T value) {
                    current_index = (current_index + 1) % buffer.size();
                    buffer[current_index] = value;
                    if (current_index == 0){
                        _full = true;
                    }
                }

                void Reset(){
                    current_index = 0;
                    _full = false;
                }

                size_t size() const {
                    return buffer.size();
                }

                int index() const {return current_index;}

           private:
                size_t current_index = 0;
                bool _full = false;
                std::array<T, N> buffer;
       };
   } // namespace Controllers
    
} // namespace PineTime
