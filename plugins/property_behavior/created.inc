<?php 
// Plugin definition
$plugin = array(
  'label' => 'Created',
  'hook_entity_save' => 'eck_created_property_entity_save'
);


function eck_created_property_entity_save(){
  dpm('Created Entity Save');
}
