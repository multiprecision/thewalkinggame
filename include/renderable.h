// renderable.h

#ifndef OGLPROJ_RENDERABLE_H_
#define OGLPROJ_RENDERABLE_H_

namespace oglproj
{

class Renderable
{
public:
    Renderable() {};
    virtual void render() = 0;
};

} // end of namespace oglproj

#endif