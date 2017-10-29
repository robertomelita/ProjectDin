#ifndef WORLDPHYSYCS_H
#define WORLDPHYSYCS_H

class worldPyshics{
    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        btAlignedObjectArray<btCollisionShape*> collisionShapes;
    public:
        worldPyshics();
        void addRigidBody(btRigidBody *body);
        void stepSimulation();
        void del();
        btDiscreteDynamicsWorld* getDynamicWorld();
        btAlignedObjectArray<btCollisionShape*> getCollisionShapes();
};

#endif