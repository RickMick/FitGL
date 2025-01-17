#pragma once
#include <Shared.h>
namespace fgl {
  classShared(SDLEventProc);
  classForward(SDLWindow);
};

#include<iostream>
#include<map>
#include<memory>
#include <functional>
#include<SDL.h>

using Callback = std::function<void()>;
using EventCallback = std::function<bool(SDL_Event const&) >;

class fgl::SDLEventProc {
public:
  using WindowId = uint32_t;
  using Name2Window = std::map<std::string, SDLWindowS>;
  using ConstNameIterator = Name2Window::const_iterator;
  using Id2Name = std::map<WindowId, std::string>;
  using ConstIdIterator = Id2Name::const_iterator;



  SDLEventProc(bool pooling = true);
  ~SDLEventProc();
  void addWindow(
    std::string const&name,
    SDLWindowS const&window);
  void removeWindow(std::string const&name);
  bool hasWindow(std::string const&name)const;
  SDLWindowS const&getWindow(std::string const&name)const;
  void operator()();
  void setIdleCallback(
    Callback const&callback = nullptr);
  bool hasIdleCallback()const;
  void callIdleCallback();
  void setEventHandler(
    EventCallback const&handler = nullptr);
  bool hasEventHandler()const;
  bool callEventHandler(SDL_Event const&eventData);
  ConstNameIterator nameBegin()const;
  ConstNameIterator nameEnd()const;
  ConstIdIterator idBegin()const;
  ConstIdIterator idEnd()const;
  size_t getNofWindows()const;

  void quit() {
    this->m_quit = true;
  }
protected:
  Callback m_idleCallback = nullptr;
  EventCallback m_eventHandler;
  bool m_pooling = true;
  bool m_running = false;
  Name2Window m_name2Window;
  Id2Name m_id2Name;
  bool m_quit = false;
};
