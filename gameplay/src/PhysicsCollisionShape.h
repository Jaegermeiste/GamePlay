#ifndef PHYSICSCOLLISIONSHAPE_H_
#define PHYSICSCOLLISIONSHAPE_H_

#include "Vector3.h"
#include "Image.h"
#include "Mesh.h"

namespace gameplay
{
    class Node;
    class Properties;

/**
 * Base physics collision shape class that all supported shapes derive from.
 */
class PhysicsCollisionShape : public Ref
{
    friend class PhysicsController;
    friend class PhysicsRigidBody;

public:

    /**
     * Defines the supported collision shape types.
     */
    enum Type
    {
        SHAPE_BOX,
        SHAPE_SPHERE,
        SHAPE_CAPSULE,
        SHAPE_MESH,
        SHAPE_HEIGHTFIELD
    };

    /**
     * Structure representing the definition of a collision shape, which is used
     * during collision shape construction time.
     *
     * Use the static methods on the PhysicsCollisionShape class to return
     * 
     */
    struct Definition
    {
        friend class PhysicsCollisionShape;
        friend class PhysicsController;
        friend class PhysicsRigidBody;
        friend class PhysicsCharacter;
        friend class PhysicsGhostObject;

    public:

        ~Definition();

    private:

        Definition();

        /**
         * Creates a PhysicsCollisionShape#Definition object from the given properties object (for the given node).
         * 
         * @param node The node to create the PhysicsCollisionShape::Definition object for.
         * @param properties The properties object to create the PhysicsCollisionShape#Definition object from.
         * @return A PhysicsCollisionShape#Definition object.
         */
        static Definition* create(Node* node, Properties* properties);

        // Shape type.
        PhysicsCollisionShape::Type type;

		// Shape data.
		struct BoxData { float center[3], extents[3]; };
		struct SphereData { float center[3]; float radius; };
		struct CapsuleData { float center[3]; float radius, height; };

        union
        {
			BoxData box;
			SphereData sphere;
			CapsuleData capsule;
			Image* heightfield;
			Mesh* mesh;
		} data;

        // Whether the shape definition is explicit, or if it is inherited from node bounds.
        bool isExplicit;

        // Whether the center position is absolute or relative to the node position.
        bool centerAbsolute;
    };

    /**
     * Returns the type of this collision shape.
     *
     * @return The collision shape type.
     */
    PhysicsCollisionShape::Type getType() const;

	/**
	 * Returns the internal bullet physics shape object.
	 *
	 * @return The bullet shape object.
	 */
	btCollisionShape* getShape() const
	{
		return _shape;
	}

    /**
     * Defines a box shape, using the bounding volume of the node it is attached to.
     *
     * @return Definition of a box shape.
     */
    static PhysicsCollisionShape::Definition box();

    /**
     * Defines a box shape, using the specified shape information and center.
     *
     * @param extents Extents of the box shape along the x, y and z axes.
     * @param center Center point of the box.
     * @param absolute True to specifiy that the given center point is an absolute position.
     *		By default the center point is treated as relative to the location of the node
     *		that the shape is attached to.
     *
     * @return Definition of a box shape.
     */
    static PhysicsCollisionShape::Definition box(const Vector3& extents, const Vector3& center = Vector3::zero(), bool absolute = false);

    /**
     * Defines a sphere shape, using the bounding volume of the node it is attached to.
     *
     * @return Definition of a sphere shape.
     */
    static PhysicsCollisionShape::Definition sphere();

    /**
     * Defines a sphere shape, using the specified shape information and center.
     *
     * @param radius Radius of the sphere.
     * @param center Center point of the sphere.
     * @param absolute True to specifiy that the given center point is an absolute position.
     *		By default the center point is treated as relative to the location of the node
     *		that the shape is attached to.
     *
     * @return Definition of a sphere shape.
     */
    static PhysicsCollisionShape::Definition sphere(float radius, const Vector3& center = Vector3::zero(), bool absolute = false);

    /**
     * Defines a capsule shape, using the bounding volume of the node it is attached to.
     *
     * @return Definition of a capsule shape.
     */
    static PhysicsCollisionShape::Definition capsule();

    /**
     * Defines a capsule shape, using the specified shape information and center.
     *
     * @param radius Radius of the capsule.
     * @param height Height of the capsule.
     * @param center Center point of the capsule.
     * @param absolute True to specifiy that the given center point is an absolute position.
     *		By default the center point is treated as relative to the location of the node
     *		that the shape is attached to.
     *
     * @return Definition of a capsule shape.
     */
    static PhysicsCollisionShape::Definition capsule(float radius, float height, const Vector3& center = Vector3::zero(), bool absolute = false);

    /**
     * Defines a heightfield shape using the specified heightfield image.
     *
     * @return Definition of a heightfield shape.
     */
    static PhysicsCollisionShape::Definition heightfield(Image* image);

    /**
     * Defines a mesh shape using the specified mehs.
     *
     * @return Definition of a mesh shape.
     */
    static PhysicsCollisionShape::Definition mesh(Mesh* mesh);

private:

    struct MeshData
    {
        float* vertexData;
        std::vector<unsigned char*> indexData;
    };

    struct HeightfieldData
    {
        float* heightData;
        unsigned int width;
        unsigned int height;
        mutable Matrix inverse;
        mutable bool inverseIsDirty;
    };

    /**
     * Constructor.
     */
    PhysicsCollisionShape(Type type, btCollisionShape* shape);

	/** 
	 * Hidden copy constructor.
	 */
	PhysicsCollisionShape(const PhysicsCollisionShape& copy);

	/**
	 * Destructor.
	 */
	~PhysicsCollisionShape();


    // Shape type
    Type _type;

    // Bullet shape object
    btCollisionShape* _shape;

    // Shape specific cached data
    union
    {
        MeshData* meshData;
        HeightfieldData* heightfieldData;
    } _shapeData;

};

}

#endif