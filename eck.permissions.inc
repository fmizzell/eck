<?php

/**
 * @param type $pid the permissions id
 */
function eck_permissions_delete_form($form, &$state, $pid){
  $path = current_path();
  $pieces = explode('delete', $path);
  $path = $pieces[0];
  
  $form['pid'] = array(
    '#type' => "value",
    '#value' => $pid
  );
  
  return confirm_form($form, "Are you sure you want to delete the permission: {$pid}", $path);
}

function eck_permissions_delete_form_submit($form, &$state){
  $values = $state['values'];
  $pid = $values['pid'];
  $permission = Permission::loadById($pid);
  $permission->delete();
  
  $path = current_path();
  $pieces = explode('delete', $path);
  $path = $pieces[0];
  
  $state['redirect'] = $path;
}

function eck_permissions($object_type = NULL, $object_id = NULL){
  
  if(!isset($object_type) && !isset($objcet_id)){
    $form = drupal_get_form("eck_permissions_receiving_object_form");
    return $form;
  }
  else{
    
    $add_form = drupal_get_form('eck_permissions_add_form', $object_type, $object_id);
    
    $header = array();
    $rows = array();

    if(isset($object_type) && isset($object_id)){
      //let's present a table with the permissions assigned already
      $permissions = array();

      if($object_type == 'role'){
        $permissions = \ECK\Permissions\Permission::loadAllByRole($object_id);
      }else if($object_type == 'user'){
        $permissions = \ECK\Permissions\Permission::loadAllByUID($object_id);
      }

      $header = array('id', 'permission', 'actions');
      $rows = array();
      $counter = 0;
      $path = current_path();
      foreach($permissions as $perm){
        $rows[$counter][] = $perm->id;
        $rows[$counter][] = $perm->permission;
        $rows[$counter][] = l("delete", "$path/delete/{$perm->id}");
        $counter++;
      }

    }

    return array('add_form' => $add_form, 'table' => array('#theme' => 'table', '#header' => $header, '#rows' => $rows));
  }
  
}

function eck_permissions_add_form($form, &$state, $object_type, $object_id){
  
  $form['object_type'] = array(
    '#type' => 'value',
    '#value' => $object_type
  );
  
  $form['object_id'] = array(
    '#type' => 'value',
    '#value' => $object_id
  );
  
  $form['permission'] = array(
    '#type' => 'textfield',
    '#title' => "Add a permission",
    '#required' => TRUE
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Add'));
  
  return $form;
}

function eck_permissions_add_form_submit($form, &$state){
  $perm = new \ECK\Permissions\Permission();
  
  $values = $state['values'];
  $perm->type = $values['object_type'];
  $perm->oid = $values['object_id'];
  $perm->permission = $values['permission'];
  
  $perm->save();
}

function eck_permissions_receiving_object_form($form, &$state){
  
  $form['object_type'] =
  array(
    '#type' => 'radios',
    '#title' => t('User or Role?'),
    '#options' => array('user' => "User", 'role' => "Role"),
    '#required' => TRUE,
  );
  
  $form['object_id'] =
  array(
    '#type' => 'textfield',
    '#title' => t('ID'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Go to Permissions!'));
  
  return $form;
}

function eck_permissions_receiving_object_form_submit($form, &$state){
  $values = $state['values'];
  $object_type = $values['object_type'];
  $object_id = $values['object_id'];
  
  $state['redirect'] = "admin/people/eck-permissions/$object_type/$object_id";
}

function eck_permissions_get_object_type_dependencies($object_type, $dependencies = array()){
  global $eck_system;
  $object_info = eck_get_object_type_info($object_type);
  if(array_key_exists('dependencies', $object_info)){
    foreach($object_info['dependencies'] as $d){
      $dependencies[] = $eck_system->getFromContext($d);
      $dependencies = eck_permissions_get_object_type_dependencies($d, $dependencies);
    }
  }
  
  return $dependencies;
}

function eck_permissions_get_object_id($object_type, $object){
  $object_id = "";
  
  $dependencies = eck_permissions_get_object_type_dependencies($object_type);
  $count = 0;
  foreach($dependencies as $d){
    if($count != 0){
      $object_id = "|".$object_id;
    }
    $object_id = $d->getId().$object_id;
    $count++;
  }
  
  if(!empty($object_id)){
    $object_id .= "|".$object->getId();
  }else{
    $object_id .= $object->getId();
  }
  
  return $object_id;
}

function eck_permissions_object_id_permutations($object_id){
  $permutations = array();
  //we also want to deal with general options of permissions
  $pieces = explode("|", $object_id);

  //how many possible permissions can be generated
  $binary_string = "";
  for($i = 0; $i < count($pieces); $i++){
    $binary_string .= 1;
  }

  $total_permissions = bindec($binary_string);

  for($i = 0; $i <= $total_permissions; $i++){

    $binary = decbin($i);
    $binary = strrev($binary);
    $pc = $pieces;

    for($j = 0; $j < count($pieces); $j++){
      $bit = substr($binary, $j, 1);
      if($bit == 1){
        $pc[$j] = "*";
      }
    }

    $new_object_id = implode("|", $pc);
    $permutations[] = $new_object_id;
  }
  
  return $permutations;
}

function eck_permissions_eck_access($op = NULL, $object_type = NULL, $object = NULL, $account = NULL){
  $allow = FALSE;
  
  //first lets get all the permissions set for this user
  $all_permissions = array();
  $permissions = \ECK\Permissions\Permission::loadAllByUID($account->uid);
  $all_permissions = array_merge($all_permissions, $permissions);
  
  $roles = $account->roles;
  foreach($roles as $rid => $role){
    $permissions = \ECK\Permissions\Permission::loadAllByRole($rid);
    $all_permissions = array_merge($all_permissions, $permissions);
  }
  
  if(empty($all_permissions)){
    return $allow;
  }
  
  //this is the only complex scenario if we have an object we need to create a string for it,
  //and check for wildcards
  $permutations = array();
  if(isset($object)){
    $object_id = eck_permissions_get_object_id($object_type, $object);
    $permutations = eck_permissions_object_id_permutations($object_id);
  }
  
  //we have to check and see if any of the permissions match our possibilities
  foreach($all_permissions as $p){
    $permission_string = $p->permission;
    
    //lets check all possible permissions with wildcards
    foreach(array('*', $op) as $operation){
      foreach(array('*', $object_type) as $obj_type){
        $possible_permission = "{$op} {$obj_type}";
        if(strcmp($permission_string, $possible_permission) == 0){ $allow = TRUE; break; }

        foreach($permutations as $permutation){
          $possible_permission_with_perm = "{$possible_permission}:{$permutation}";
          if(strcmp($permission_string, $possible_permission_with_perm) == 0){ $allow = TRUE; break; }
        }
      }
    } 
  }

  return $allow;
}

/*
//CODE RELATED TO BEHAVIORS AFFECTING PERMISSIONS
//should behaviors care about anything but entity permissions?
//I guess we'll play it by ear
if($object_type == "entity"){

  $entity_type_name = $object->entityType();
  $entity_type = EntityType::loadByName($entity_type_name);
  $behavior_access = eck_property_behavior_invoke_plugin($entity_type, 
    'permissions', array('op' => $op, 'entity' => $object, 'permissions' => $perms, 
    'account' => $account)); 

  foreach($behavior_access as $access){
    if($access){
      $allow = TRUE;
    }
  }
}
*/

function eck_permissions_eck_access_default($op = NULL, $object_type = NULL, $object = NULL, $account = NULL){
  $master = "eck * {$object_type}";
  $perm = "eck {$op} {$object_type}";

  $allow = user_access($perm, $account) || user_access($master, $account);
  return $allow;
}
