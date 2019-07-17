//==============================================================================
/*
Software License Agreement (BSD License)
Copyright (c) 2003-2016, CHAI3D.
(www.chai3d.org)

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

* Neither the name of CHAI3D nor the names of its contributors may
be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

\Origianl Author	Francois Conti (CNormalMap.h)
\Modified by		Sunghwan Shin
\version   3.2.0 $Rev: 1869 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#ifndef CHeightMapH
#define CHeightMapH
//------------------------------------------------------------------------------
#include "materials/CTexture2d.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
namespace chai3d {
	//------------------------------------------------------------------------------

	//==============================================================================
	/*!
	\file   CHeightMap.h

	\brief
	Implements a height map for texture
	*/
	//==============================================================================

	//------------------------------------------------------------------------------
	class cHeightMap;
	typedef std::shared_ptr<cHeightMap> cHeightMapPtr;
	//------------------------------------------------------------------------------

	//==============================================================================
	/*!
	\class      cHeightMap
	\ingroup    materials

	\brief
	This class implements a Height map

	\details
	This class  implements a Height map which is used for haptic and graphic
	bump mapping rendering.
	*/
	//==============================================================================
	class cHeightMap : public cTexture2d
	{
		//--------------------------------------------------------------------------
		// CONSTRUCTOR & DESTRUCTOR:
		//--------------------------------------------------------------------------

	public:

		//! Constructor of cHeightMap.
		cHeightMap();

		//! Destructor of cHeightMap.
		virtual ~cHeightMap();

		//! Shared cHeightMap allocator.
		static cHeightMapPtr create() { return (std::make_shared<cHeightMap>()); }


		//--------------------------------------------------------------------------
		// PUBLIC METHODS:
		//--------------------------------------------------------------------------

	public:

		//! This method creates a copy of itself.
		cHeightMapPtr copy();

		//! This method creates a Height map from a texture object.
		virtual void createMap(cTexture1dPtr a_texture);

		//! This method creates a Height map from an image object.
		virtual void createMap(cImagePtr a_image);

		//! This method flips normals along U and/or V axis.
		void flip(const bool a_flipU, const bool a_flipV);
	};

	//------------------------------------------------------------------------------
} // namespace chai3d
  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
#endif
  //------------------------------------------------------------------------------