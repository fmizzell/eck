<?php

/**
 * @return (array) information about the different property types in the system 
 */
function hook_eck_property_types(){
  $property_types['property_type_machine_name'] =
  array(
    'label' => "Property Type",
    'class' => "PropertyTypeClass"
  );
  
  return $property_types;
}

function hook_eck_widget_types(){
  $widget_types['widget_type_machine_name'] =
  array(
    'class' => "WidgetTypeClass"
  );
  
  return $property_types;
}

function hook_eck_object_types(){
  
}