/*
	worldview/Model.hh
	------------------
*/

#ifndef WORLDVIEW_MODEL_HH
#define WORLDVIEW_MODEL_HH

// Standard C++
#include <vector>

// plus
#include "plus/string_fwd.hh"

// worldview
#include "worldview/Objects.hh"


namespace worldview
{
	
	namespace V = Vectoria;
	
	class Scene;
	
	typedef std::size_t Camera;
	
	V::XMatrix WorldToEyeTransform( const Scene& scene, std::size_t camera );
	V::XMatrix EyeToWorldTransform( const Scene& scene, std::size_t camera );
	
	V::XMatrix EyeToPortTransform();
	
	class Scene
	{
		private:
			std::vector< Context >  itsContexts;
			std::vector< Camera  >  itsCameras;
			
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
			
			std::size_t AddSubcontext( std::size_t          super,
			                           const plus::string&  name,
			                           const V::XMatrix&    offset,
			                           const V::XMatrix&    inv );
			
			const std::vector< Camera >& Cameras() const  { return itsCameras; }
			      std::vector< Camera >& Cameras()        { return itsCameras; }
	};
	
}

#endif
