<?php 
// Plugin definition
$plugin = array(
  'label' => 'Created',
  'entity_save' => 'eck_created_property_entity_insert'
);


function eck_created_property_entity_insert($property, $vars){
  $entity = $vars['entity'];
  if(isset($entity->is_new) && $entity->is_new){
    $entity->{$property} = time();
  }
}
