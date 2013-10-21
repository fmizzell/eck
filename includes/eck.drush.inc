<?php


function eck_drush_command(){
  $commands = array();
  
  $commands['eck'] = array(
    'description' => "The master command for eck interaction :)",
    'arguments' => array(
      'object_type' => 'what eck object type are we dealing with (entity_type, bundle, entity, etc)',
      'operation' => 'what are we doing with the object',
      'id' => 'the identifier of the object we are operating on'
    )
  );

  return $commands;
}

function drush_eck($object_type, $operation, $id = NULL) {
  $drush_interface = new ECK\UI\Drush\DrushInterface();
  $eck = new ECK\Core\ECK($drush_interface);
  $eck->init($operation, $object_type, $id);
}



