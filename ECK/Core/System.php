<?php
namespace ECK\Core;
use ECK\Core\Property;
use ECK\Core\KeyValues;
use Symfony\Component\DependencyInjection\ContainerBuilder;
use Symfony\Component\DependencyInjection\Definition;

class System{
  
  //this is the initial interaction with the system, here is where the 
  //user lets the system know which operation and in which object(s) the
  //operation should be performed
  private $initial_user_input;
  private $interface;
  private $main_object_type;
  
  /*//System language
  private $operation;
  private $object_type;
  private $object_id;
  
  //The system transform its language into acutual objects or clases
  private $operation_object;
  private $object_class;
  private $object;*/
  
  //the user interface interacts with the system by providing needed information 
  //so the system can operate
  private $user_input;
  
  private $container;
  
  /**
   * The system starts working when getting some sort of input from the user
   * The system doesn't know how interactions with user are happening, and
   * that is why we need an interface to translate user input, and to 
   * communicate back with the user
   * 
   * @param iUserInterface $interface
   * @param type $user_input 
   */
  public function __construct(iUserInterface $interface, $user_input){
    $this->interface = $interface;
    $this->initial_user_input = $user_input;
    $this->user_input = array();
    $this->container = new ContainerBuilder();
    
    global $eck_system;
    $eck_system = $this;
  }
  
  public function setObject($object_type, $id){
    $info = eck_get_object_type_info($object_type);
    $class = $info['class'];
    
    $container = $this->container;
    $container->setParameter("{$object_type}.class", $class);
    $container->setParameter("{$object_type}_factory.class", "{$class}Factory");

    $container->setDefinition("{$object_type}_factory", new Definition(
        "%{$object_type}_factory.class%"
    ));
    $container->setDefinition($object_type, new Definition(
        "%{$object_type}.class%",
        array($id)
    ))->setFactoryService(
        "{$object_type}_factory"
    )->setFactoryMethod(
        'get'
    );
  }
  
  public function setOperation($operation){
    $info = eck_get_operation_info($operation);
    $class = $info['class'];
    $this->container->register('operation', $class);
  }
  
  public function setMainObjectType($object_type){
    $this->main_object_type = $object_type;
  }
  
  public function getMainObjectType(){
    return $this->main_object_type;
  }
  
  public function getFromContext($var){
    return $this->container->get($var);
  }
  
  public function getMainObject(){
    $object = NULL;
    if($this->container->hasDefinition($this->main_object_type)){
      $object = $this->container->get($this->main_object_type);
    }
    return $object;
  }
  
  public function doThings(){
    //So first lets see if we can get some valid info from the user input
    $this->interface->parseInitialUserInput($this, $this->initial_user_input);
    
    $object = $this->getMainObject();
    $operation = $this->getFromContext('operation');
    
    $requirements = $operation->getRequirements();
      
    //if we have requirements, then we need to ask the user
    if($requirements){
      //because in some cases this operation is asynchronous, we don't do anything
      //after this, instead we wait for the interface to call user input themselves
      //and the perform operation method
      return $this->interface->getRequirementsFromUser($this, $requirements);
    }
    //otherwise we can proceed to perform the operation
    else{
      //after the operation is displayed we give any info returned from the operation
      //to the interface to see if they would like to process it as an output
      //so here is where we return whatever the interface wants
      return $this->performOperation();
    }
  }
  
  public function performOperation(){
    $output = $this->getFromContext('operation')->operate($this->user_input);
    if(!$output){
      $output = "Done!";
    }
    return $this->interface->outputToUser($this, $output);
  }
  
  public function addUserInput($value, $key = NULL){
    if($key){
      $this->user_input[$key] = $value;
    }else{
      $this->user_input[] = $value;
    }
  }
  
  /*public function setObjectType($value){
    $this->object_type = $value;
  }
  
  public function setOperation($value){
    $this->operation = $value;
  }
  
  public function setObjectId($value){
    $this->object_id = $value;
  }
  */
  
  /**
   * The system understand three things
   * operations, object_types, and objects
   * but for us to work with them we translate simple strings into actual 
   * objects and classes
   */ 
  /*private function translateLanguage(){
    //first we get the object class as it is required for both operations
    //and actual objects
    $this->setObjectClass();
    
    //then if we have an id, we get the acutal object, as this is will be 
    //required by the operation if it was given
    if($this->object_id){
      $this->setObject();
    }
    
    //and finally we set up the operation object
    $this->setOperationObject();
  }*/
  
  /*private function setOperationObject(){
    
  }
  
  private function setObject(){
    $this->setObjectClass($this->object_type);
    $class = $this->object_class;
    $this->object = $class::loadFromId($this->object_id);
  }
  
  private function setObjectClass(){
    $info = eck_get_object_type_info($this->object_type);
    $this->object_class = $info['class'];
  }*/
}
