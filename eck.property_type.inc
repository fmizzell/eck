<?php

function eck_get_property_types(){
  return module_invoke_all("eck_property_types");
}

function eck_get_property_type($property_type){
  $property_types = eck_get_property_types();
  
  return $property_types[$property_type];
}

function eck_get_property_type_schema($property_type){
  $property_type = eck_get_property_type($property_type);
  return $property_type['class']::schema();
}
