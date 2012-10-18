--------------------------------------------------

EffectParamCounter =
{
	Int = 0,
	Float = 0,
	Vector = 0,
	Matrix = 0,
	Texture = 0,
}

function CountEffectParam( effectInfo )
	
	local paramType

	paramType = effectInfo.Type

	if EffectParamCounter[paramType] ~= nil then

		EffectParamCounter[paramType] = EffectParamCounter[paramType] + 1
		
	else

		error ("Unknown effect parameter type ", effectInfo.Type)

	end
end

--------------------------------------------------

entry = CountEffectParam

dofile(DirScript .. "effect_param_list.lua")

print("Number of Int parameters = "..EffectParamCounter.Int)

print("Number of Float parameters = "..EffectParamCounter.Float)

print("Number of Vector parameters = "..EffectParamCounter.Vector)

print("Number of Matrix parameters = "..EffectParamCounter.Matrix)

print("Number of Texture parameters = "..EffectParamCounter.Texture)

neAllocEffectParam()

--------------------------------------------------

entry = neRegisterEffectParam

dofile(DirScript .. "effect_param_list.lua")

--------------------------------------------------


