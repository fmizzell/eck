<?php
// Plugin definition
$plugin = array(
  'label' => "Author",
  'entity_insert' => 'eck_author_property_entity_insert',
  'property_info' => 'eck_author_property_property_info'
);

function eck_author_property_entity_insert($property, $vars){
  $entity = $vars['entity'];
  global $user;
  $entity->{$property} = $user->uid;
}

function eck_author_property_property_info($property, $vars){
  $vars['properties'][$property]['type'] = 'user';
  
  return $vars;
}
