<?php
// Plugin definition
$plugin = array(
  'label' => "Changed",
  'entity_save' => 'eck_changed_property_entity_save'
);

function eck_changed_property_entity_save($entity, $property, $value = NULL){
  $entity->{$property} = time();
}
