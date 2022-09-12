#pragma once
#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

using namespace std;

//класс таймер измеряет время выполнения кода в деструкторе
class Timer {
 public:
  std::chrono::_V2::system_clock::time_point start, end; //временные точки начала и конца
  Timer(); // конструктор инициализация начала
  ~Timer(); // деструктор вывод (end - start) времени выполнения
};

#endif //!_TIMER_H_
