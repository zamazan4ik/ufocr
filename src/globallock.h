#ifndef GLOBALLOCK_H
#define GLOBALLOCK_H

class Unlocker
{
public:
    explicit Unlocker(bool locked);
    ~Unlocker();
private:
    bool unlock;
};

class GlobalLock
{
public:
    static GlobalLock *instance();
    bool lock();
    void unlock();
    bool isLocked();
private:
    GlobalLock();
    GlobalLock(const GlobalLock &);
private:
    static GlobalLock *m_instance;
};

#endif // GLOBALLOCK_H
