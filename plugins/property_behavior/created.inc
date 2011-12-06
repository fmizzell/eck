<?php 
// Plugin definition
$plugin = array(
  'label' => 'Created',
  'entity_insert' => 'eck_created_property_entity_insert'
);


function eck_created_property_entity_insert($entity, $property, $value = NULL){
  $entity->{$property} = time();
}
