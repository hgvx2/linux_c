#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#define DECLARE_SINGLETON(T) private: \
T(); \
static T *m_pInstance; \
public: \
static T *GetInstance(); \
static void ReleaseInstance();


#define IMPLEMENT_SINGLETON(T) T *T::m_pInstance = 0; \
T* T::GetInstance() \
{ \
  if (0 == m_pInstance) \
   { \
      m_pInstance = new T; \
   } \
   return m_pInstance; \
} \
void T::ReleaseInstance() \
{ \
  if (m_pInstance) \
    { \
       delete m_pInstance; \
       m_pInstance = 0; \
    } \
} \
T::T()


// 1个参数的单实例
#define DECLARE_SINGLETON_1(T, P, p1) private: \
T(P p1); \
static T *m_pInstance; \
public: \
static T *GetInstance(P p1); \
static void ReleaseInstance();


#define IMPLEMENT_SINGLETON_1(T, P, p1) T *T::m_pInstance = 0; \
T* T::GetInstance(P p1) \
{ \
  if (0 == m_pInstance) \
   { \
      m_pInstance = new T(p1); \
   } \
   return m_pInstance; \
} \
void T::ReleaseInstance() \
{ \
  if (m_pInstance) \
    { \
       delete m_pInstance; \
       m_pInstance = 0; \
    } \
} \
T::T(P p1)

// 2个参数的单实例
#define DECLARE_SINGLETON_2(T, P, p1, Q, q2) private: \
T(P p1, Q q2); \
static T *m_pInstance; \
public: \
static T *GetInstance(P p1, Q q2); \
static void ReleaseInstance();


#define IMPLEMENT_SINGLETON_2(T, P, p1, Q, q2) T *T::m_pInstance = 0; \
T* T::GetInstance(P p1, Q q2) \
{ \
  if (0 == m_pInstance) \
   { \
      m_pInstance = new T(p1, q2); \
   } \
   return m_pInstance; \
} \
void T::ReleaseInstance() \
{ \
  if (m_pInstance) \
    { \
       delete m_pInstance; \
       m_pInstance = 0; \
    } \
} \
T::T(P p1, Q q2)

#endif // SINGLETON_HPP


/*using
 
 * head file
 class CTest
 * {
 *   DECLARE_SINGLETON(CTest)
 *   public:
 *   ~CTest();
 * }
 
 * source file
 * IMPLEMENT_SINGLETON(CTest)
 * {
 *   // initialize same member
 * }
 
 
 */
