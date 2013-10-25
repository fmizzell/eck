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
  $system = new \ECK\Core\System(new \ECK\UI\Drush\DrushInterface(), 
          array($object_type, $operation, $id));
  //and then we tell it to do things
  return $system->doThings();
}



