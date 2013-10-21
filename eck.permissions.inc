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
        $permissions = Permission::loadAllByRole($object_id);
      }else if($object_type == 'user'){
        $permissions = Permission::loadAllByUID($object_id);
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
  $perm = new Permission();
  
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

function eck_permissions_eck_access($op, $object_type, $object, $account){
  $allow = FALSE;
  
  //we are only dealing with object specific permissions, general permission are
  //handled by eck
  if(isset($object)){
    $roles = $account->roles;
    $perms = array();

    //first we want the user specific permissions
    $new = Permission::loadAllByUID($account->uid);
    $perms = array_merge($perms, $new);
    
    foreach($roles as $rid => $role){
      $new = Permission::loadAllByRole($rid);
      $perms = array_merge($perms, $new);
    }

    
    foreach($perms as $p){
      $perm = $p->permission;
      $object_id = eck_permissions_object_id($object_type, $object);
      
      $cperm = "{$op} {$object_type}:{$object_id}";
      if(strcmp($perm, $cperm) == 0){
        $allow = TRUE;
      }
      
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
        
        $cperm = "{$op} {$object_type}:{$new_object_id}";
        if(strcmp($perm, $cperm) == 0){
          $allow = TRUE;
        }
      }
    }
    
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
  }

  return array($allow);
}
