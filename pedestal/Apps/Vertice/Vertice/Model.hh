/*	================
 *	Vertice/Model.hh
 *	================
 */

#ifndef VERTICE_MODEL_HH
#define VERTICE_MODEL_HH

#include <map>
#include <string>
#include <vector>

#include "Vertice/Objects.hh"


namespace Vertice
{
	
	namespace V = Vectoria;
	
	class Scene;
	
	class Camera
	{
		private:
			std::size_t itsContextIndex;
		
		public:
			Camera() : itsContextIndex( 0 )  {}
			
			Camera( std::size_t index ) : itsContextIndex( index )  {}
			
			V::XMatrix WorldToEyeTransform( const Scene& model ) const;
			V::XMatrix EyeToWorldTransform( const Scene& model ) const;
			
			V::XMatrix EyeToPortTransform() const;
			V::XMatrix PortToEyeTransform() const;
			
			std::size_t ContextIndex() const  { return itsContextIndex; }
	};
	
	class PointLight
	{
		
	};
	
	class Scene
	{
		private:
			std::vector< Context >                itsContexts;
			std::map< std::string, std::size_t >  itsNameIndex;
			
			std::vector< Camera >                 itsCameras;
			std::vector< PointLight >             itsPointLights;
			
		public:
			Scene();
			
			Context const& GetContext( std::size_t index ) const;
			Context      & GetContext( std::size_t index );
			
			std::size_t GetSuperContext( std::size_t index ) const
			{
				return GetContext( index ).ParentIndex();
			}
			
			bool SuperContextExists( std::size_t index ) const
			{
				return GetSuperContext( index ) < index;  // False for root context
			}
			
			std::size_t AddSubcontext( std::size_t         super,
			                           const std::string&  name,
			                           const V::XMatrix&   offset,
			                           const V::XMatrix&   inv );
			
			const std::vector< Camera >& Cameras() const  { return itsCameras; }
			      std::vector< Camera >& Cameras()        { return itsCameras; }
	};
	
}

#endif

