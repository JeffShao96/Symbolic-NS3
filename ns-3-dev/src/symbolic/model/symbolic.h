/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef SYMBOLIC_H
#define SYMBOLIC_H
// #include "ns3/ptr.h"
// #include <list>
#include "ns3/object.h"
#include "ns3/nstime.h"

namespace ns3 {

/* ... */
class Symbolic: public Object
{

public:

  static TypeId GetTypeId (void);

  Symbolic ();
  Symbolic (uintptr_t v);
  virtual ~Symbolic (){};

  void SetMax(uintptr_t v);
  void SetMin(uintptr_t v);
  void SetMax(Time v);
  void SetMin(Time v);
  void GetSymbolic();
  Time GetSymbolicTime();
  void PrintRange();
  void PrintRange(char const* name);
  void PrintSignedRange();
  void PrintSignedRange(char const* name);
  uintptr_t GetUpperBound();
  uintptr_t GetLowerBound();

  uintptr_t m_symbolic;
  uintptr_t m_min;
  uintptr_t m_max;

  friend Symbolic operator - (const Symbolic & l, const Symbolic & r);
  friend Symbolic operator - (const Time & l, const Symbolic & r);
  friend Symbolic operator - (const Symbolic &  l, const Time &r);
  friend Symbolic operator + (const Symbolic & l, const Symbolic & r);
  friend Symbolic operator + (const Time & l, const Symbolic & r);
  friend Symbolic operator + (const Symbolic &  l, const Time &r);

};

inline Symbolic operator - (const Symbolic & l, const Symbolic & r)
{
  return Symbolic (l.m_symbolic - r.m_symbolic);
}

inline Symbolic operator - (const Time & l, const Symbolic & r)
{
  return Symbolic (l.GetTimeStep() + r.m_symbolic);
}

inline Symbolic operator - (const Symbolic &  l, const Time &r)
{
  return Symbolic (l.m_symbolic - r.GetTimeStep());
}

inline Symbolic operator + (const Symbolic & l, const Symbolic & r)
{
  return Symbolic (l.m_symbolic + r.m_symbolic);
}

inline Symbolic operator + (const Time & l, const Symbolic & r)
{
  return Symbolic (l.GetTimeStep() + r.m_symbolic);
}

inline Symbolic operator + (const Symbolic &  l, const Time &r)
{
  return Symbolic (l.m_symbolic + r.GetTimeStep());
}

}

#endif /* SYMBOLIC_H */

