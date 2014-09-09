<?php
/**
 * @file
 * Classes for all the different objects used in ECK.
 */

class DBObject implements Iterator {
  // Wheteher this object was loaded or just created.
  public $is_new;

  // Iterator variable.
  private $position;


  // The database table where the objects exist.
  private $table;

  private $vars;
  private $data;

  private $primary_keys;
  private $serialize;

  /**
   * Constructor.
   */
  protected function __construct($table) {
    $this->serialize = array();
    $this->is_new = TRUE;

    // Iterator variable.
    $this->position = 0;

    $data = array();

    // Is this a real table? If it is, check it.
    if ($schema = drupal_get_schema($table)) {
      $this->table = $table;
      $this->primary_keys = $schema["primary key"];
      $this->vars = array_keys($schema['fields']);

      // Do we want to handle searialized variables by default? let's do it
      // and wait for some critizism.
      foreach ($schema['fields'] as $name => $field) {
        if (array_key_exists('serialize', $field) && $field['serialize']) {
          $this->serialize[] = $name;
        }
      }
      foreach ($this->vars as $var) {
        if ($schema['fields'][$var]['type'] != "serial") {
          $this->data[$var] = NULL;
        }
      };
    }
    else {
      // @todo throw an exception.
    }
  }

  /**
   * Magic method.
   */
  public function __set($var, $value) {
    if (in_array($var, $this->vars)) {
      $this->data[$var] = $value;
    }
  }

  /**
   * Magic method.
   */
  public function __get($var) {
    if (property_exists($this, $var)) {
      return $this->{$var};
    }

    return $this->data[$var];
  }

  /**
   * Magic method.
   */
  public function __isset($name) {
    return isset($this->data[$name]);
  }

  /**
   * Magic method.
   */
  public function __unset($name) {
    unset($this->data[$name]);
  }

  /**
   * Save.
   */
  public function save(){

    //before we save, lets serialize the properties that require it
    foreach($this->serialize as $property){

      $this->{$property} = drupal_json_encode($this->{$property});
    }

    if($this->is_new){
      $this->id =
      db_insert($this->table)
      ->fields($this->data)
      ->execute();
    }else{
      //well I need to know what the primary id is to set up the condition;
      $primary_key = $this->primary_keys[0];
      db_update($this->table)
      ->condition($primary_key, $this->{$primary_key}, '=')
      ->fields($this->data)
      ->execute();
    }

    //now that we are done saving lets deserialize in case that for some
    //reason we will continue manipulating the properties
    foreach($this->serialize as $property){
      $this->{$property} = drupal_json_decode($this->{$property});
    }

    $this->is_new = FALSE;
  }

  /**
   * Load.
   *
   * @param string $property
   *   The property we will use to search for the record.
   * @param mixed $value
   *   the value the property should match.
   */
  protected function load($property, $value){
    $result =
    db_select($this->table, 't')
    ->fields('t')
    ->condition($property, $value,'=')
    ->execute()
    ->fetchAssoc();

    if($result){
      foreach($result as $property => $value){
        if(in_array($property, $this->serialize)){
          $value = drupal_json_decode($value);
        }
        $this->{$property} = $value;
      }
      // We should only set the is_new flag as false if we loaded something.
      $this->is_new = FALSE;
    }
  }

  /**
   * Delete.
   */
  public function delete(){
    //we can only deleted if its a loaded object, or if it has been saved
    if(!$this->is_new){
      $query = db_delete($this->table);
      $primary_key = $this->primary_keys[0];
      $query->condition($primary_key, $this->{$primary_key}, '=');
      $query->execute();

      // Should we delete the data from the object.. not sure.
      // For right now lets just set it back to new.
      $this->is_new = TRUE;
    }
  }

  /**
   * From Iterator Interface.
   */
  function rewind() {
      $this->position = 0;
  }

  /**
   * From Iterator Interface.
   */
  public function current() {
    return $this->data[$this->key()];
  }

  /**
   * From Iterator Interface.
   */
  public function key() {
    return $this->vars[$this->position];
  }

  /**
   * From Iterator Interface.
   */
  public function next() {
    ++$this->position;
  }

  /**
   * From Iterator Interface.
   */
  public function valid() {
    if (in_array($this->position, array_keys($this->vars))) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
}


class EntityType extends DBObject{

  // If an entity type is new, we can create its table from the current data of
  // the object, but if this is a loaded object, we need to actually keep
  // track of the changes happening so we can modify the already existing table
  // appropiately.
  private $changes;

  public function __construct(){
    parent::__construct('eck_entity_type');
    $this->properties = array();
    $this->changes = array();
  }

  /**
   * Add a new property to the entity type.
   *
   * @param string $name
   *   The name.
   * @param string $label
   *   A label.
   * @param string $type
   *   The type of the property.
   * @param string $behavior
   *   A behavior to attach to the property.
   */
  public function addProperty($name, $label, $type, $behavior = NULL, $schema = NULL){
    if(!$this->is_new){
      $this->recordFieldChange('add', $name);
    }

    $p = $p2 = $this->properties;

    // @todo check that type is an actual type.
    $p[$name] = array(
      'label' => $label,
      'type' => $type,
      'behavior' => $behavior,
      'schema' => $schema,
    );

    $this->properties = $p;

    // If the property did not exist already, let the behaviors execute some
    // code.
    if (!array_key_exists($name, $p2)) {
      eck_property_behavior_invoke_plugin($this, 'property_add',
        array('name' => $name, 'property' => $p[$name], 'entity_type' => $this));
    }
  }

  /**
   * Remove a property.
   *
   * @param string $name
   *   The name of the property.
   */
  public function removeProperty($name){
    $p = $this->properties;
    if (array_key_exists($name, $p)) {

      // Let the behaviors execute some code.
      eck_property_behavior_invoke_plugin($this, 'property_remove',
      array('name' => $name, 'property' => $p[$name], 'entity_type' => $this));

      unset($p[$name]);
      $this->properties = $p;
      if (!$this->is_new) {
        $this->recordFieldChange('remove', $name);
      }
    }
  }

  /**
   * Change the behavior of a property.
   *
   * @param string $name
   *   The name of the property.
   *
   * @param string $behavior
   *   the name of the behavior.
   */
  public function changeBehavior($name, $behavior){
    $p = $this->properties;
    // @todo check that type is an actual type.
    if (array_key_exists($name, $p)) {
      $p[$name]['behavior'] = $behavior;
      // @todo look at this more closelly, does the behavior change really
      // affect the property cache?
      entity_property_info_cache_clear();
    }
    else {
      // @todo add exception if the property does not exist.
    }

    $this->properties = $p;
  }

  /**
   * Remove behavior.
   *
   * @param string $name
   *   The name of the behavior.
   */
  public function removeBehavior($name){
    $this->changeBehavior($name, NULL);
  }

  private function recordFieldChange($op, $name){
    //If it is not new we need to keep track of stuff
    if(!$this->is_new){
      $p = $this->properties;
      $c = $this->changes;
      switch ($op) {
        case 'add':
          // If the property does not exist already add keep track.
          if (!array_key_exists($name, $p)) {
            $c[$op][] = $name;
          }
          break;
        case  'remove':
          // If there is an add in the changes take it out, otherwise add a
          // remove.
          if(array_key_exists('add', $c)){
            $key = array_search($name, $c['add']);
            if ($key != FALSE) {
              unset($c['add'][$key]);
            }
          }
          else {
            $c[$op][] = $name;
          }
          break;

      }
      $this->changes = $c;
    }
  }

  /**
   * Save.
   */
  public function save(){
    if($this->is_new){
      module_load_include('inc', 'eck', 'eck.entity_type');
      $schema = eck__entity_type__schema($this);
      db_create_table("eck_{$this->name}", $schema);

      // Allow other modules to respond to the creation of entity types.
      module_invoke_all('eck_entity_type_insert', $this);
    }
    else {
      // Modify the already existing table in accordance with the
      // recorded changes.
      if (array_key_exists('add', $this->changes)) {
        foreach ($this->changes['add'] as $name) {
          // First lets get the record.
          $properties = $this->properties;
          $property = $properties[$name];

          // Now we check to see whether it is a default or a custom property
          // it is not custom so lets get the schema and add the field.
          $schema = eck_get_property_type_schema($property['type']);
          db_add_field("eck_{$this->name}", $name, $schema);
        }
      }
      if (array_key_exists('remove', $this->changes)) {
        foreach ($this->changes['remove'] as $name) {
          db_drop_field("eck_{$this->name}", $name);
        }
      }

      // Allow other modules to respond to the change of entity types.
      module_invoke_all('eck_entity_type_update', $this);
    }

    parent::save();

    global $eck_entity_types_cache;

    $cache_enabled = isset($eck_entity_types_cache)?TRUE:FALSE;

    if($cache_enabled) {
      $eck_entity_types_cache->reset();
    }

    variable_set('eck_clear_caches', TRUE);
  }
  /**
   * Delete.
   */
  public function delete() {
    // Delete all the bundles from this entity type.
    $bundles = Bundle::loadByEntityType($this);
    foreach ($bundles as $bundle) {
      $bundle->delete();
    }

    parent::delete();
    db_drop_table('eck_' . $this->name);

    // Allow other modules to respond to the deletion of entity types.
    module_invoke_all('eck_entity_type_delete', $this);

    global $eck_entity_types_cache;

    $cache_enabled = isset($eck_entity_types_cache)?TRUE:FALSE;

    if($cache_enabled) {
      $eck_entity_types_cache->reset();
    }
    variable_set('eck_clear_caches', TRUE);
  }

  public static function loadByName($name) {
    return EntityType::loadAll($name);
  }

  public static function loadAll($entity_type_name = '', $reset = FALSE) {
    $entity_types = array();

    global $eck_entity_types_cache;

    $cache_enabled = isset($eck_entity_types_cache)?TRUE:FALSE;

    if($cache_enabled) {
      if ($reset) {
        $eck_entity_types_cache->reset();
      }

      $entity_types = $eck_entity_types_cache->get();
    }

    if (empty($entity_types)) {
      $entity_types = array();
      $results = db_select('eck_entity_type', 't')
        ->fields('t', array('name'))
        ->execute();

      foreach($results as $result){
        $name = $result->name;
        $entity_type = new EntityType();
        $entity_type->load('name', $name);
        $entity_types[$name] = $entity_type;
      }

      if($cache_enabled) {
        $eck_entity_types_cache->set($entity_types);
      }
    }
    if (!$entity_type_name) {
      return $entity_types;
    }
    else if (array_key_exists($entity_type_name, $entity_types)) {
      return $entity_types[$entity_type_name];
    }
    else {
     return array();
    }
  }
}

class Bundle extends DBObject {

  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct('eck_bundle');
    $this->config = array();
  }

  /**
   * Create a machine name.
   */
  private function createMachineName() {
    $this->machine_name = "{$this->entity_type}_{$this->name}";
  }

  /**
   * Create a label.
   */
  private function createLabel() {
    $name = $this->name;
    $pieces = explode("_", $name);
    $final = array();
    foreach ($pieces as $piece) {
      $final[] = ucfirst($piece);
    }

    $this->label = implode(" ", $final);
  }

  /**
   * Save the entity type.
   */
  public function save() {
    // Lets do some checks before the bundle is saved.
    if (isset($this->entity_type) && isset($this->name)) {

      $save = TRUE;

      // Lets set the machine name.
      $this->createMachineName();

      //if this bundle is_new we need to check that it does not exist
      //@todo we just need to change the field in the db to be unique
      if ($this->is_new) {
        $bundle = Bundle::loadByMachineName($this->machine_name);
        if($bundle && !$bundle->is_new){
          $save = FALSE;
        }
      }

      if (!isset($this->label)) {
        $this->createLabel();
      }

      if ($save) {
        parent::save();
        global $eck_bundles_cache;

        $cache_enabled = isset($eck_bundles_cache)?TRUE:FALSE;

        if($cache_enabled) {
          $eck_bundles_cache->reset();
        }

        variable_set('eck_clear_caches', TRUE);
      }else{
        //@todo throw some error
      }
    }
    else {
      // If the name an entity type are not set, we can not save the bundle.
      // @todo throw soem error or exception.
    }
  }

  /**
   * Delete the entity type.
   */
  public function delete() {
    // First delete all of the entities of this bundle.
    $query = new EntityFieldQuery();

    $query->entityCondition('entity_type', $this->entity_type, '=')
      ->entityCondition('bundle', $this->name, '=');

    $results = $query->execute();

    if (!empty($results)) {
      $ids = array_keys($results[$this->entity_type]);
      entity_delete($this->entity_type, $ids);
    }

    // Then we delete the bundle (field_instances).
    field_attach_delete_bundle($this->entity_type, $this->name);

    parent::delete();

    global $eck_bundles_cache;

    $cache_enabled = isset($eck_bundles_cache)?TRUE:FALSE;

    if($cache_enabled) {
      $eck_bundles_cache->reset();
    }

    variable_set('eck_clear_caches', TRUE);
  }

  /**
   * This method returns a bundle object.
   *
   * @param string $machine_name
   *   A string composed of the entity type name and the bundle name.
   *   "{$entity_type_name}_{$bundle_name}"
   */
  public static function loadByMachineName($machine_name){
    return Bundle::loadAll($machine_name);
  }

  public static function loadAll($machine_name = NULL, $reset = FALSE) {
    $bundles = array();

    global $eck_bundles_cache;

    $cache_enabled = isset($eck_bundles_cache)?TRUE:FALSE;

    if($cache_enabled) {
      if ($reset) {
        $eck_bundles_cache->reset();
      }

      $bundles = $eck_bundles_cache->get();
    }

    if (empty($bundles)) {
      $bundles = array();
      //@todo move this to a general function.
      $results = db_select('eck_bundle', 't')
        ->fields('t', array('machine_name'))
        ->execute();

      foreach($results as $result){
        $name = $result->machine_name;
        $bundle = new Bundle();
        $bundle->load('machine_name', $name);
        $bundles[$name] = $bundle;
      }

      if ($cache_enabled) {
        $eck_bundles_cache->set($bundles);
      }
    }

    if (!$machine_name) {
      return $bundles;
    }
    else if (array_key_exists($machine_name, $bundles)) {
      return $bundles[$machine_name];
    }
    else {
     return array();
    }
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
   * @param string $field_type
   *   The type of field to add. One of the keys as defined by any field module
   *   using hook_field_info.
   *
   * @param array $options
   *   This is an optional array. Its properties can include:
   *   _use existing_: If TRUE and if a 'field_name' property is specified in
   *   the 'field' property below and the field already exists, then a new
   *   instance will be created using the existing field. All specified 'field
   *   options provided other then the field name will be ignored. If FALSE,
   *   and an existing field is found then a new field_name will be generated.
   *   TRUE by default.
   *   _field_: all options accepted by field_create_field(). Defaults will be
   *   used for each property that is omitted. Most defaults come from
   *   field_create_field(). Default 'field_name'generation.
   *
   * @return bool
   *   The $instance array with the id property filled in as returned by
   *   field_create_instance().
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
      $field += array(
        'field_name' => substr('field_' . $field_type, 0, 28) . '_' . $iter);
    }

    // Create a new field if the field name is unique over active and
    // disabled fields.
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
    if (array_key_exists('instance', $options)) {
      $instance += $options['instance'];
    }
    return field_create_instance($instance);
  }
}

class ECKEntity extends Entity{

  //this flag only gets set so we can make our properties public again
  //before we save the object;
  protected $ignore_validation;

  protected $property_values = array();

  public function __construct(array $values = array(), $entityType = NULL){

    $this->ignore_validation = FALSE;

    parent::__construct($values, $entityType);

    //I have this stupid crap.. fixing drupals mess ups.
    $entity_type_name = $this->entityType();
    $entity_type = EntityType::loadByName($entity_type_name);
    $properties = $entity_type->properties;
    $property_names = array_keys($properties);

    foreach($property_names as $pn){
      $value = $this->{$pn};
      unset($this->$pn);
      $this->{$pn} = $value;
    }
  }

  public function __set($name, $value) {
    //let's implement non restrictive validation.
    //if it is a property validate, if it isn't just set it

    // Try to load the entityType and property information.
    if ($entity_type_name = $this->entityType()) {
      if (($entity_type = EntityType::loadByName($entity_type_name)) && is_object($entity_type)) {
        if ($properties = $entity_type->properties) {
          $property_names = array_keys($properties);
        }
      }
    }

    //TODO: should look into loading the entity info through entity_get_info() and determining validation that way.
    //      Currently we don't validate when the ECKEntityType has not been instantiated yet. Not sure if it is needed though.
    if (isset($property_names) && in_array($name, $property_names) && !$this->ignore_validation) {
      $property_type = $properties[$name]['type'];
      $property_type_class = eck_get_property_type_class($property_type);

      if (!isset($value)) {
        $schema = $property_type_class::schema();
        if (isset($schema['default'])) {
          $this->property_values[$name] = $schema['default'];
        }
      }
      elseif ($property_type_class::validate($value)) {
        $this->property_values[$name] = $value;
      }
      else {
        throw new Exception("Invalid value {$value} for property {$name} of type {$property_type} in
        Entity type: {$entity_type_name}");
      }

    } else {
      $this->{$name} = $value;
    }
  }


  public function __get($name) {
    if (array_key_exists($name, $this->property_values)){
      return $this->property_values[$name];
    }
    else if (isset($this->{$name})){
      return $this->{$name};
    }
    else if ($field = field_info_field($name)) {
      if ($this->entityType) {
        if ($bundle = $this->bundle()) {
          if (in_array($bundle, $field['bundles'][$this->entityType])) {
            $this->{$name} = array();
            return $this->{$name};
          }
        }
      }
    }
    return NULL;
  }

  public function __isset($name) {
    if(array_key_exists($name, $this->property_values)){
      return TRUE;
    }else if(isset($this->{$name})){
      return TRUE;
    }
    return FALSE;
  }

  public function save(){
    //going back to public properties
    $this->ignore_validation = TRUE;
    foreach($this->property_values as $property => $value){
      $this->{$property} = $value;
    }
    return parent::save();
  }
}

class ECKCache {

  private $id;
  private $data;

  private $no_cache;

  public function __construct($id) {
    $this->id = $id;
    $this->no_cache = FALSE;

    if (!db_table_exists('cache_eck')) {
      $this->no_cache = TRUE;
    }

  }

  public function set($data) {
    if($this->no_cache) return;

    cache_set($this->id, $data, "cache_eck");
    $this->data = $data;
  }

  public function get() {
    if($this->no_cache) return NULL;

    if($this->data) {
      return $this->data;
    }
    else{
      $cached_data = cache_get($this->id, "cache_eck");
      if($cached_data) {
        $this->data = $cached_data->data;
        return $this->data;
      }
      else {
        return NULL;
      }
    }
  }

  public function reset() {
    if($this->no_cache) return;

    $this->data = NULL;
    cache_clear_all($this->id, 'cache_eck');
  }
}
