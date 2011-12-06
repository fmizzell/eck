<?php
// Plugin definition
$plugin = array(
  'label' => "Author",
  'entity_insert' => 'eck_author_property_entity_insert'
);

function eck_author_property_entity_insert($entity, $property, $value = NULL){
  global $user;
  $entity->{$property} = $user->uid;
}
