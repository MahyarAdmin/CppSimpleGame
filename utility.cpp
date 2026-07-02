#include <chrono>

namespace Util{
    namespace Algorithm{
        inline float clampData(float min, float data, float max) {
            if(data < min)return min;
            if(data > max)return max;
            return data;
        }
    }

    namespace Time{
        template<class T>
        class Timer{
            std::chrono::high_resolution_clock::time_point TimeCounter, TimerTemp;
            std::chrono::duration<T> TimeDiff;
            void setDuration(void) {
                TimeDiff = std::chrono::duration_cast<std::chrono::duration<T>>(TimeCounter - TimerTemp);
            }
            public:
                Timer(const std::chrono::high_resolution_clock::time_point &InitTime = std::chrono::high_resolution_clock::time_point{}
                , const std::chrono::duration<T> &InitDuration = std::chrono::duration<T>::zero()) 
                : TimeCounter(InitTime), TimerTemp(InitTime), TimeDiff(InitDuration) {};
                std::chrono::high_resolution_clock::time_point getTimeCount(void) const {
                    return TimeCounter;
                }
                std::chrono::high_resolution_clock::time_point getTimeTemp(void) const {
                    return TimerTemp;
                }
                void SetCount(std::chrono::high_resolution_clock::time_point &Count) {
                    TimeCounter = Count;
                }
                void SetTemp(std::chrono::high_resolution_clock::time_point &Temp) {
                    TimerTemp = Temp;
                }
                void SetLast(void) {
                    TimerTemp = TimeCounter;
                }
                void SetNow(void) {
                    TimeCounter = std::chrono::high_resolution_clock::now();
                }
                void SetAndSave(void) {
                    SetLast();
                    SetNow();
                }
                T getDuration(void) {
                    setDuration();
                    return TimeDiff.count();
                }
        };
    }
}