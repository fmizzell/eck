<?php
// Plugin definition
$plugin = array(
  'label' => "Changed",
  'entity_save' => 'eck_changed_property_entity_save'
);

function eck_changed_property_entity_save($property, $vars){
  $entity = $vars['entity'];
  $entity->{$property} = time();
}
