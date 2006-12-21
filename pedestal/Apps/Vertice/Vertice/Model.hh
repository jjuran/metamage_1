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
	
	class Model;
	
	class Camera
	{
		private:
			std::size_t fContextIndex;
		
		public:
			Camera() : fContextIndex( 0 )  {}
			
			Camera( std::size_t index ) : fContextIndex( index )  {}
			
			V::XMatrix WorldToEyeTransform( const Model& model ) const;
			V::XMatrix EyeToWorldTransform( const Model& model ) const;
			
			V::XMatrix EyeToPortTransform() const;
			V::XMatrix PortToEyeTransform() const;
			
			std::size_t ContextIndex() const  { return fContextIndex; }
	};
	
	class PointLight
	{
		
	};
	
	class Model
	{
		private:
			std::vector< Context >                fContexts;
			std::map< std::string, std::size_t >  fNameIndex;
			
			std::vector< Camera >                 myCameras;
			std::vector< PointLight >             fPointLights;
			
		public:
			Model();
			
			Context const& GetContext( std::size_t index ) const;
			Context      & GetContext( std::size_t index );
			
			Moveable const& GetSubcontext( std::size_t index ) const;
			Moveable      & GetSubcontext( std::size_t index );
			
			std::size_t GetSuperContext( size_t index ) const;
			
			bool SuperContextExists( std::size_t index ) const;
			
			std::size_t AddSubcontext( std::size_t         super,
			                           const std::string&  name,
			                           const V::XMatrix&   offset,
			                           const V::XMatrix&   inv );
			
			const std::vector< Camera >& Cameras() const  { return myCameras; }
			      std::vector< Camera >& Cameras()        { return myCameras; }
	};
	
}

#endif

