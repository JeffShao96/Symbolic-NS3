/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "symbolic.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include <cstdlib>
#include "/home/s2e/s2e/s2e.h"

namespace ns3 {

/* ... */
NS_LOG_COMPONENT_DEFINE ("Symbolic");

NS_OBJECT_ENSURE_REGISTERED (Symbolic);

TypeId 
Symbolic::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Symbolic")
    .SetParent<Object> ()
    .SetGroupName ("Symbolic")
    .AddConstructor<Symbolic> ()
    .AddAttribute ("Max", "The Symbolic Max Value",
                   UintegerValue (0xffff),
                   MakeUintegerAccessor (&Symbolic::m_max),
                   MakeUintegerChecker<uintptr_t> ())
    .AddAttribute ("Min", "The Symbolic Min Value",
                   UintegerValue (0),
                   MakeUintegerAccessor (&Symbolic::m_min),
                   MakeUintegerChecker<uintptr_t> ())
  ;
  return tid;
}

Symbolic::Symbolic()
  :
   m_symbolic(0),
   m_min(0),
   m_max(0xffffffff)

{
  NS_LOG_FUNCTION_NOARGS ();
}

Symbolic::Symbolic(uintptr_t v)
  :
   m_symbolic(v),
   m_min(0),
   m_max(0xffffffff)

{
  NS_LOG_FUNCTION_NOARGS ();
}


void
Symbolic::SetMax(uintptr_t v)
{
    m_max=v;
}

void
Symbolic::SetMin(uintptr_t v)
{
    m_min=v;
}

void
Symbolic::SetMax(Time v)
{
    m_max=v.GetTimeStep();
}

void
Symbolic::SetMin(Time v)
{
    m_min=v.GetTimeStep();
}

void
Symbolic::GetSymbolic()
{   
  if(s2e_is_symbolic(&m_symbolic,sizeof(m_symbolic)) != 1)
  {
    s2e_make_symbolic(&m_symbolic,sizeof(m_symbolic),"symbolic_value");
    if(m_symbolic<m_min)
    {
      s2e_kill_state(0,"Out of Range, Lower");
    }
    else if(m_symbolic>m_max)
    {
      s2e_kill_state(0,"Out of Range, Upper");
    }
  }
}

Time 
Symbolic::GetSymbolicTime()
{
    GetSymbolic();
    return Time(m_symbolic);
}

void 
Symbolic::PrintRange()
{
  uintptr_t upper;
  uintptr_t lower;
  s2e_get_range(m_symbolic,&lower,&upper);
  s2e_printf("The range of Symbolic Variables is %ld,%ld",lower,upper);
}

void 
Symbolic::PrintRange(char const* name)
{
  uintptr_t upper;
  uintptr_t lower;
  s2e_get_range(m_symbolic,&lower,&upper);
  s2e_printf("The range of %s Variables is %ld,%ld",name,lower,upper);
  s2e_printf("The range of %s Variables is %lu,%lu",name,lower,upper);
}


uintptr_t 
Symbolic::GetUpperBound()
{
  uintptr_t upper;
  uintptr_t lower;
  s2e_get_range(m_symbolic,&lower,&upper);
  return upper;
}

uintptr_t 
Symbolic::GetLowerBound()
{
  uintptr_t upper;
  uintptr_t lower;
  s2e_get_range(m_symbolic,&lower,&upper);
  return lower;
}

}

