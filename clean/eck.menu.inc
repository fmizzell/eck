<?php

function eck__bundle__menu($entity_type) {
  
  return array();
  // Create the menus relavant to types.
  $path = eck__entity_type__path();
  $menu = array();
  
  // EDIT an entity type
  $menu["{$path}/{$entity_type->name}/edit"] = array(
    'title' => "Edit",
    'description' => "Edit the '{$entity_type->label}' entity type",
    'page callback' => 'drupal_get_form',
    'page arguments' => array("eck__entity_type__form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'entity_type', $entity_type),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
  );

  // DELETE an entity type
  $menu["{$path}/{$entity_type->name}/delete"] = array(
    'title' => "Delete",
    'description' => "Delete the '{$entity_type->label}' Entity Type",
    'page callback' => "drupal_get_form",//"eck__entity_type__delete",
    'page arguments' => array("eck__entity_type__delete_form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("delete", "entity_type", $entity_type),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 3,
  );
  
  //MANAGE Entity Type Properties
  $menu["{$path}/{$entity_type->name}/property"] = array(
    'title' => "Manage properties",
    'description' => "Manage the properties of the {$entity_type->label} entity type",
    'page callback' => "eck__properties",
    'page arguments' => array($entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 4,
  );
  
  //Add a property
  $menu["{$path}/{$entity_type->name}/property/add"] = array(
    'title' => "Add property",
    'description' => "Add a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",//"eck__entity_type__delete",
    'page arguments' => array("eck__property_add__form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
    'type' => MENU_LOCAL_ACTION,
    'weight' => 4,
  );
    
  //Activate a default property
  $menu["{$path}/{$entity_type->name}/property/%/activate"] = array(
    'title' => "Activate default property",
    'page callback' => "eck__activate_default_property",
    'page arguments' => array($entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.default_properties.inc',
  );
    
  //Dectivate a default property
  $menu["{$path}/{$entity_type->name}/property/%/deactivate"] = array(
    'title' => "Activate default property",
    'page callback' => "eck__deactivate_default_property",
    'page arguments' => array($entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.default_properties.inc',
  );
  
  //Edit a property
  $menu["{$path}/{$entity_type->name}/property/%/edit"] = array(
    'title' => "Edit property",
    'description' => "Edit a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__property_edit__form", $entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );
  
  //Delete a property
  $menu["{$path}/{$entity_type->name}/property/%/delete"] = array(
    'title' => "Delete property",
    'description' => "Delete a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__property_delete__form", $entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );
    
  //Modify (add/change/delete) the behavior on a property
 //Edit a property
  $menu["{$path}/{$entity_type->name}/property/%/behavior"] = array(
    'title' => "Edit property",
    'description' => "Modify the behavior of a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__property_behavior__form", $entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );
  
  // LIST bundles of an entity type.
  $menu["{$path}/{$entity_type->name}"] = array(
    'title' => "{$entity_type->label}" ,
    'description' => "View all the bundles for '{$entity_type->label}'",
    'page callback' => "eck__bundle__list",
    'page arguments' => array(3),
    'access callback' => 'eck_access',
    'access arguments' => array('list', 'bundle'),
    'file' => 'eck.bundle.inc'
  );
  
  $menu["{$path}/{$entity_type->name}/list"] = array(
    'title' => 'Bundle List',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100,
  );

  
  //ADD bundles to an entity type
  $menu["{$path}/{$entity_type->name}/add"] = array(
    'title' => "Add bundle ",
    'description' => "Add a(n) new '{$entity_type->label} Bundle'",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__add', 3),
    'access callback' => 'eck_access',
    'access arguments' => array('create', 'bundle'),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 0,
    'file' => 'eck.bundle.inc'
  );

  module_load_include('inc', 'eck', 'eck.entity');
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
    $menu = array_merge($menu, eck__entity__menu($entity_type, $bundle));
  }

  return $menu;
}

function eck__entity__menu($entity_type, $bundle) {
  $path = eck__entity_type__path();
  $menu = array();
  
  // DELETE Bundle
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/delete"] = array(
    'title' => "Delete",
    'page callback' => "drupal_get_form",//"eck__bundle__delete",
    'page arguments' => array('eck__bundle__delete_form', 3, 4),
    'access callback' => 'eck_access',
    'access arguments' => array('delete', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK
  );

  // Managing a bundle's properties as extra fields
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%"] = array(
    'title arguments' => array(6),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__manage_extra_field_form', $entity_type->name, $bundle->name, 6),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
  );
  
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/edit"] = array(
    'title' => 'Edit',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/widget-type"] = array(
    'title' => 'Widget Type',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__extra_field_widget_form', $entity_type->name, $bundle->name, 6),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );
  
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/remove"] = array(
    'title' => 'Remove',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__remove_extra_field_form', $entity_type->name, $bundle->name, 6),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 10,
  );
  
  $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);
  
  // OVERVIEW Entity
  $menu[$admin_info['path']] = array(
    'title' => "{$bundle->label}",
    'description' => "View all entites of type {$entity_type->label} with bundle {$bundle->label}",
    'page callback' => "eck__entity__list",
    'page arguments' => array($entity_type->name, $bundle->name),
    'access callback' => 'eck_access',
    'access arguments' => array('list', 'entity'),
    'weight' => 0,
    'file' => 'eck.entity.inc'
  );

  $menu[$admin_info['path']."/list"] = array(
    'title' => "Entity List",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100
  );
  
  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);

  foreach($crud_info as $action => $info){
    
    switch($action){
      case 'add':
        $perm_op = 'create';
        break;
      case 'edit':
        $perm_op = 'update';
        break;
      default:
        $perm_op = $action;
        break;
    }
    
    $perm_args = array($perm_op, 'entity');
    
    $action_label = ucfirst($action);
    $args = array();
    
    if(array_key_exists('entity_id', $info)){
      $args[] = $info['entity_id'];
      $perm_args[] = $info['entity_id'];
    }
    
    $args = array_merge(array($entity_type->name, $bundle->name), $args);

    $menu[$info['path']] = array(
      'title' => "{$action_label} {$bundle->label}",
      'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
      'page callback' => "eck__entity__{$action}",
      'page arguments' => $args,
      'access callback' => 'eck_access',
      'access arguments' => $perm_args,
      'load arguments' => array($entity_type->name),
      'file' => 'eck.entity.inc',
    );

    if($action == 'view'){
      
      $menu[$info['path']."/view"] = array(
        'title' => "View",
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => 0
      );
    }
  }
  
  return $menu;
}
