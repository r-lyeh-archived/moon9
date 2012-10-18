entry 
{
	Name		= "vL0D",	--light 0 direction
	Id			= 1,
	Type		= "Vector",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry 
{
	Name		= "vL0C",	--light 0 colour
	Id			= 2,
	Type		= "Vector",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry 
{
	Name		= "vL0A",	--light 0 ambient
	Id			= 3,
	Type		= "Vector",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry 
{
	Name		= "dAMB",	--ambient colour
	Id			= 4,
	Type		= "Int",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "mSPM",	--spherical map matrix
	Id			= 5,
	Type		= "Matrix",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "vSPM",	--spherical map material
	Id			= 6,
	Type		= "Vector",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "tSPM",	--test projector texture
	Id			= 7,
	Type		= "Texture",
	SubType		= "File",
	File		= "spheremap2.bmp",
--	File		= "envmapligh.jpg",
--	File		= "gratelamp_flare.jpg",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "mPJ0",	--projector matrix 0
	Id			= 8,
	Type		= "Matrix",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "tPJ0",	--test projector texture
	Id			= 9,
	Type		= "Texture",
	SubType		= "File",
	File		= "bumpmap.bmp",--"checker.bmp",
--	File		= "checker.bmp",
	UpdateMode	= "BeginLoop",
}

----------------------------------------

entry
{
	Name		= "vODF",	-- object diffuse
	Id			= 10,
	Type		= "Vector",
	UpdateMode	= "EachRoot",
}

----------------------------------------

entry
{
	Name		= "vOAM",	-- object ambient
	Id			= 11,
	Type		= "Vector",
	UpdateMode	= "EachRoot",
}
