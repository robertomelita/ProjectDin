#ifndef WORLDPHYSYCS_H
#include <btBulletDynamicsCommon.h>
#define WORLDPHYSYCS_H

class worldPhysics{
    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        btAlignedObjectArray<btCollisionShape*> collisionShapes;
    public:
        worldPhysics();
        void addRigidBody(btRigidBody *body);
        void stepSimulation();
        void del();
        btDiscreteDynamicsWorld* getDynamicWorld();
        btAlignedObjectArray<btCollisionShape*> getCollisionShapes();
};

#endif