-- Notifies the engine when a new global property is set.

do
 local _tostring=tostring
 local tostring=function(a)
  if type(a)=="table" then   
   table.foreach(a, SetConfigOption) -- notify engine about each table element
  else
   return _tostring(a)
  end
 end


 local set=function (t,name,value)  
  if type(value)=="table" then
    SetConfigOption(name,"table") --notify the engine a new table has been defined
    tostring(value) -- notify the engine bout each table element
  else
    SetConfigOption(value,name,"string") -- notify engine a new global property has been defined
  end
 end
 setmetatable(getfenv(),{__index=g,__newindex=set})
end