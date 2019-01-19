// Author: xcw
// Email:  xcw_coder@qq.com
//2018年11月23日20:15:33
#pragma once

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};