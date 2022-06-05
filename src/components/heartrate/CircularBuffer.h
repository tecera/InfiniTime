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

                /**
                 * @brief Check if there have been at least N values added to the buffer
                 * 
                 * @return bool
                 */
                bool IsFull() const {
                    return _full;
                }

                /**
                 * @brief Get a value from the buffer
                 * 
                 * @param index The index of the value to get, 0-indexed, where 0 is the oldest value in the buffer
                 * @return T 
                 */
                const T& Get(size_t index) const {
                    return buffer[index % buffer.size()];
                }

                /**
                 * @brief Add a new value to the buffer, overwriting the oldest value. Moves the start of the index one forward.
                 * 
                 * @param value The new value
                 */
                void Append(const T& value) {
                    buffer[current_index] = value;
                    current_index = (current_index + 1) % buffer.size();
                    if (current_index == 0){
                        _full = true;
                    }
                }

                /**
                 * @brief Reset the index to 0 and set full to false
                 * 
                 */
                void Reset(){
                    current_index = 0;
                    _full = false;
                }

                /**
                 * @brief Size of the buffer
                 * 
                 * @return size_t 
                 */
                size_t size() const {
                    return buffer.size();
                }

                const std::array<T, N>& GetData() const {
                    return buffer;
                }

                /**
                 * @brief Returns the current internal index
                 * 
                 * @return int 
                 */
                int index() const {return current_index;}

           private:
                size_t current_index = 0;
                bool _full = false;
                std::array<T, N> buffer;
       };
   } // namespace Controllers
    
} // namespace PineTime
