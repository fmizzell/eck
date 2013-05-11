<?php
// Plugin definition
$plugin = array(
  'label' => "Author",
  'entity_save' => 'eck_author_property_entity_save',
  'property_info' => 'eck_author_property_property_info',
  'permissions' => 'eck_author_property_permissions'
);

function eck_author_property_entity_save($property, $vars){
  $entity = $vars['entity'];
  if(isset($entity->is_new) && $entity->is_new && empty($entity->{$property})){
    global $user;
    $entity->{$property} = $user->uid;
  }
}

function eck_author_property_property_info($property, $vars){
  $vars['properties'][$property]['type'] = 'user';
  return $vars;
}

function eck_author_property_permissions($property, $vars){
  $op = $vars['op'];
  $perms = $vars['permissions'];
  $account = $vars['account'];
  $entity = $vars['entity'];
  
  $allow = FALSE;
  foreach($perms as $perm){
    $p = $perm->permission;
    $property = $entity->{$property};
    $uid = $account->uid;
    if((strcmp($p, "{$op} own entity") == 0) && ( $property == $uid)){
      $allow = TRUE;
    }
  }
  return $allow;
}
