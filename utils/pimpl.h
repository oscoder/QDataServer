#ifndef UTILS_PIMPL_H
#define UTILS_PIMPL_H

#include <QtCore/QtGlobal>

namespace Utils {

/*!
    \brief Use this class as a pointer and you don't need to read documentation.

    This class is supposed to be a simplification and replacement of Qt pimpl
    (private implementation) mechanism.
    Instead of using Qt macros Q_DECLARE_PRIVATE, Q_DECLARE_PUBLIC, Q_D and Q_Q
    which use d_ptr and q_ptr variables and declare d_func and q_func functions
    you can declare just one variable per class.
    In public class that will access private class create variable
    <tt>DQPtr\<MyPrivateClass\>&nbsp;d</tt> and in private class that will
    access public class create <tt>DQPtr\<MyPublicClass\>&nbsp;q</tt>. Then use
    both variables, <b>d</b> and <b>q</b> as a normal pointer.
 */
template <class T>
class DQPtr
{
public:
    //! Constructs a class that operates on given class \a p.
    DQPtr(T *p) : p(p)
    { Q_ASSERT(p != 0); }

public:
    //@{
    //! \name Imitate a pointer
    T *operator->()
    { return p; }

    const T *operator->() const
    { return p; }

    operator T *()
    { return p; }

    operator const T *() const
    { return p; }
    //@}

    //! Use this in combination with delete
    T *detach()
    {
        Q_ASSERT(p != 0);
        T *const p = this->p;
        this->p = 0;
        return p;
    }

private:
    T *p;
};

} //namespace Utils

#endif // UTILS_PIMPL_H
