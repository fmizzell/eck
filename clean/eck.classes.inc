<?php

class Bundle{ //extends DBObject{

  public function __construct(){
    parent::__construct('eck_bundle');
    $this->config = array();
  }

  private function createMachineName(){
    $this->machine_name = "{$this->entity_type}_{$this->name}";
  }

  private function createLabel(){
    $name = $this->name;
    $pieces = explode("_", $name);
    $final = array();
    foreach($pieces as $piece){
      $final[] = ucfirst($piece);
    }

    $this->label = implode(" ", $final);
  }

  public function save(){
    //Lets do some checks before the bundle is saved
    if(isset($this->entity_type) && isset($this->name)){

      $save = TRUE;
      //we are good
      //@todo we should check that the entity type is a proper
      //entity type object

      //Lets set the machine name
      $this->createMachineName();

      //if this bundle is_new we need to check that it does not exist
      //@todo we just need to change the field in the db to be unique
      if($this->is_new){
        $bundle = Bundle::loadByMachineName($this->machine_name);
        if($bundle && !$bundle->is_new){
          $save = FALSE;
        }
      }

      if(!isset($this->label)){
        $this->createLabel();
      }

      if($save){
        parent::save();
      }else{
        //@todo throw some error
      }

    }else{
      //if the name an entity type are not set, we can not save
      //the bundle
      //@todo throw soem error or exception
    }
  }

  /**
   * This method returns a bundle object
   * @param $machine_name
   *  (String) A string composed of the entity type name and the bundle name
   *  "{$entity_type_name}_{$bundle_name}"
   */
  public static function loadByMachineName($machine_name){
    return Bundle::loadAll($machine_name);
  }

  public static function loadAll($machine_name = NULL, $reset = FALSE){
    static $bundles;

    if ($reset) {
      $bundles = NULL;
      // Clear all languages.
      cache_clear_all('bundles', 'cache_eck', TRUE);
    }

    if (!isset($bundles)) {
      if ($cached = cache_get("bundles", 'cache_eck')) {
        $bundles = $cached->data;
      }
      else {
        $bundles = array();
        //@todo move this to a general function
        $results = db_select('eck_bundle', 't')
          ->fields('t', array('machine_name'))
          ->execute();

        foreach($results as $result){
          $name = $result->machine_name;
          $bundle = new Bundle();
          $bundle->load('machine_name', $name);
          $bundles[$name] = $bundle;
        }
        cache_set("bundles", $bundles, 'cache_eck');
      }
    }

    if (!$machine_name) return $bundles;

    if (isset($bundles[$machine_name])) return $bundles[$machine_name];
  }

  public static function loadByEntityType($entity_type){
    $entity_bundles = &drupal_static(__FUNCTION__, array());

    $entity_type_name = $entity_type->name;

    if (!isset($entity_bundles[$entity_type_name])) {
      $entity_bundles[$entity_type_name] = array();
      $bundles = Bundle::loadAll();
      $entity_bundles[$entity_type_name] = array_filter(
        $bundles,
        function ($bundle) use ($entity_type_name) {
          return $entity_type_name == $bundle->entity_type;
        }
      );
    }

    return (isset($entity_bundles[$entity_type_name])) ? $entity_bundles[$entity_type_name] : array();
  }

 /**
  * Adds a field to this bundle.
  *
  *
  * @param $field_type
  *   The type of field to add. One of the keys as defined by any field module using hook_field_info.
  *
  * @param $options
  *   This is an optional array. Its properties can include:
  *   - use existing: If TRUE and if a 'field_name' property is specified in the 'field'
  *     property below and the field already exists, then a new instance will be created
  *     using the existing field. All specified 'field' options provided other then the field
  *     name will be ignored. If FALSE, and an existing field is found then a new field_name
  *     will be generated. TRUE by default.
  *   - field: all options accepted by field_create_field(). Defaults will be used for each
  *     property that is omitted. Most defaults come from field_create_field().
  *     Default 'field_name' generation:
  *     - field_name: 'field_' + field type + #, where # is one more then the number of fields
  *       of that type that already exist active or inactive.
  *   - instance: all options accepted by field_create_instance(). Defaults will be used for
  *     each property that is omitted. 'bundle' and 'entity_type' properties are ignored because
  *     they come from the bundle info. The field_name property is either generated or taken from
  *     the field properties.
  *
  * @return
  *   The $instance array with the id property filled in as returned by field_create_instance().
  *
  * @throws FieldException
  *
  * See: @link field Field API data structures @endlink.
  */
  public function addField($field_type, $options = array()) {
    // Check that the field type is known.
    $field_info = field_info_field_types($field_type);
    if (!$field_info) {
      throw new FieldException(t('Attempt to add a field of unknown type %type.', array('%type' => $field_type)));
    }

    // By default use an existing field if one is found.
    $options += array('use existing' => TRUE);
    // Set field options and merge in any provided field settings.
    $field = array('type' => $field_type);
    if (!empty($options['field'])) {
      $field += $options['field'];
    }

    // Retrieve existing fields of this type.
    $field_type_fields = field_read_fields(array('type' => $field_type), array('include_inactive' => TRUE));

    // Formulate a default field name.
    if (empty($field['field_name']) || (isset($field_type_fields[$field['field_name']]) && !$options['use existing'])) {
      $iter = count($field_type_fields) + 1;
      $field += array('field_name' => substr('field_' . $field_type, 0, 28) . '_' . $iter);
    }

    // Create a new field if the field name is unique over active and disabled fields.
    if (!isset($field_type_fields[$field['field_name']])) {
      field_create_field($field);
    }

    // Add an instance of the field to this bundle.
    $instance = array(
      'field_name' => $field['field_name'],
      'entity_type' => $this->entity_type,
      'bundle' => $this->name,
    );
    // Merge any provided properties and settings.
    if(array_key_exists('instance', $options)){
      $instance += $options['instance'];
    }
    return field_create_instance($instance);
  }
}

//
