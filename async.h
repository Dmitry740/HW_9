#pragma once

#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

enum class State { Static, Dynamic };

void print_file(const std::vector<std::string>& queue);

void print_log(const std::vector<std::string>& queue);

class Observer {
 public:
  virtual ~Observer() = default;
  virtual void update(const std::vector<std::string>& queue) = 0;
};

class Observable {
 public:
  virtual ~Observable() = default;
  virtual void subscribe(std::shared_ptr<Observer> obs) = 0;
};

class BlockState : public Observable {
 public:
  void subscribe(std::shared_ptr<Observer> obs) override {
    m_subs.push_back(std::move(obs));
  }

  void set_state(const int& n) {
    m_n = n;

    switch (m_state) {
      case State::Dynamic:
        while (std::cin) {
          std::cin >> m_command;
          if (m_command == "{") {
            ++m_dyn;
            m_num = 0;
          }
          if (m_command == "}") {
            --m_dyn;
          }
          if (m_command != "{" && m_command != "}") {
            if (m_num < m_n && m_dyn > 0) {
              m_queue.push_back(m_command);
              ++m_num;
            }
          }
          if (m_num == (m_n + 1) || m_dyn == 0) {
            notify();
            m_queue.clear();
            m_num = 0;
            m_state = State::Static;
            set_state(m_n);
          }
        }
        break;
      case State::Static:
        while (std::cin) {
          std::cin >> m_command;
          if (m_command == "{") {
            ++m_dyn;
          }
          if (m_dyn == 0 && m_num < m_n && std::cin) {
            m_queue.push_back(m_command);
            ++m_num;
          }
          if (m_dyn == 0 && m_num < m_n && !std::cin) {
            notify();
            m_queue.clear();
            m_state = State::Static;
            m_num = 0;
          }
          if (m_dyn > 0) {
            notify();
            m_queue.clear();
            m_state = State::Dynamic;
            m_num = 0;
            set_state(m_n);
          }
          if (m_dyn == 0 && !std::cin) {
            notify();
            m_queue.clear();
            m_state = State::Static;
            m_num = 0;
          }
          if (m_dyn == 0 && m_num == m_n) {
            notify();
            m_queue.clear();
            m_state = State::Static;
            m_num = 0;
          }
        }
        break;
    }
  }
  void notify() {
    for (const auto& q : m_subs) {
      q->update(m_queue);
    }
  }

 private:
  State m_state{State::Static};
  int m_n{0};
  int m_num{0};
  std::string m_command;
  int m_dyn{0};
  std::vector<std::string> m_queue;
  std::vector<std::shared_ptr<Observer>> m_subs;
};

class Log : public Observer {
 public:
  Log() : log([this]() { golog(); }) {}

  ~Log() { log.join(); }

  void update(const std::vector<std::string>& queue) override {
    {
      std::unique_lock<std::mutex> lock(mut);
      m_queues.push_back(queue);
    }
    var.notify_all();
  }

  void golog() {
    std::unique_lock<std::mutex> lock(mut);

    for (;;) {
      for (const auto& queue : m_queues) {
        print_log(queue);
      }
      m_queues.clear();
      var.wait(lock);
    }
  }

 private:
  std::thread log;
  std::vector<std::vector<std::string>> m_queues;
  std::mutex mut;
  std::condition_variable var;
};

class File : public Observer {
 public:
  File() : file1([this]() { gofile(); }), file2([this]() { gofile(); }) {}

  ~File() {
    file1.join();
    file2.join();
  }
  void update(const std::vector<std::string>& queue) override {
    {
      std::unique_lock<std::mutex> locks(mutf);
      m_queuesf.push_back(queue);
    }
    varf.notify_all();
  }

  void gofile() {
    std::unique_lock<std::mutex> locks(mutf);

    for (;;) {
      for (const auto& queue : m_queuesf) {
        print_file(queue);
      }
      m_queuesf.clear();
      varf.wait(locks);
    }
  }

 private:
  std::thread file1;
  std::thread file2;
  std::vector<std::vector<std::string>> m_queuesf;
  std::mutex mutf;
  std::condition_variable varf;
};
