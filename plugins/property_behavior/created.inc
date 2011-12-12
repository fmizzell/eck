<?php 
// Plugin definition
$plugin = array(
  'label' => 'Created',
  'entity_insert' => 'eck_created_property_entity_insert'
);


function eck_created_property_entity_insert($property, $vars){
  $entity = $vars['entity'];
  $entity->{$property} = time();
}
