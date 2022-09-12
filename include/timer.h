#pragma once
#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

using namespace std;

//����� ������ �������� ����� ���������� ���� � �����������
class Timer {
 public:
  std::chrono::_V2::system_clock::time_point start, end; //��������� ����� ������ � �����
  Timer(); // ����������� ������������� ������
  ~Timer(); // ���������� ����� (end - start) ������� ����������
};

#endif //!_TIMER_H_
