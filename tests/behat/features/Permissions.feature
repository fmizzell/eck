@api @permissions
Feature: Permissions
  As a site administrator
  I want to control who can do what with entity types, bundles and entities
  so users don't get themselves in trouble.

  @setup
  Scenario Outline: This is a set up step
    Given I am logged in as a user with the "administrator" role
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with <type_label>
    And I fill in "edit-entity-type-name" with <type>
    And I fill in "edit-bundle-label" with <bundle_label>
    And I fill in "edit-bundle-name" with <bundle>
    And I check "Title"
    And I press the "Save" button
    And I visit <link>
    And I click <add_link>
    And I fill in "Title" with <entity_title>
    And I press the "Save" button

    Examples: 
      | type_label | type      | bundle_label | bundle | entity_title   | link                                       | add_link  |
      | "Vehicle"  | "vehicle" | "Car"        | "car"  | "Toyota Prius" | "/admin/structure/entity-type/vehicle/car" | "Add Car" |
      | "Animal"   | "animal"  | "Dog"        | "dog"  | "Snoopy"       | "/admin/structure/entity-type/animal/dog"  | "Add Dog" |

  @entity-type
  Scenario: Only allowed users can access the entity type's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "Use the administration pages and help" permission
    And I visit "/admin/structure"
    Then I should not see the text "Entity types"
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure"
    And I click "Entity types"
    Then I should get a "200" HTTP response

  @entity-type
  Scenario: Only allowed users can add entity types from the overview page
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "Add entity type"
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,Add Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    Then I should get a "200" HTTP response

  @entity-type
  Scenario: Only allowed users can delete entity types
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "delete"
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,Delete Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    And I click "delete" in the "Vehicle" row
    Then I should get a "200" HTTP response

  @bundle
  Scenario: Users without the right permission can not access the bundle's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the text "Vehicle"
    And I should not see the link "Vehicle"

  @bundle
  Scenario Outline: Users with the right permission can access the bundle's overview page (global)
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link <type_label>
    When I click <type_label>
    Then I should get a "200" HTTP response

    Examples: 
      | type_label |
      | "Vehicle"  |
      | "Animal"   |

  @bundle
  Scenario: Users with the right permission can access the bundle's overview page (specific)
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,View List of Vehicle Bundles" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link "Vehicle"
    And I should not see the link "Animal"
    And I should see the text "Animal"
    When I click "Vehicle"
    Then I should get a "200" HTTP response

  @bundle
  Scenario: Users without the right permission can not add bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "Add bundle"

  @bundle
  Scenario Outline: Users with the right permission can add bundles from the overview page (global)
    Given I am logged in as a user with the "View Bundle Lists,Add Bundles" permissions
    And I visit <link>
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response

    Examples: 
      | link                                   |
      | "/admin/structure/entity-type/vehicle" |
      | "/admin/structure/entity-type/animal"  |

  @bundle
  Scenario: Users with the right permission can add bundles from the overview page (specific)
    Given I am logged in as a user with the "View Bundle Lists,Add Vehicle Bundles" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "Add bundle"

  @bundle
  Scenario: Users without the right permission can not delete bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "delete"

  @bundle
  Scenario Outline: Users with the right permission can delete bundles from the overview page (global)
    Given I am logged in as a user with the "View Bundle Lists,Delete Bundles" permissions
    And I visit <link>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples: 
      | link                                   |
      | "/admin/structure/entity-type/vehicle" |
      | "/admin/structure/entity-type/animal"  |

  @bundle
  Scenario: Users with the right permission can delete bundles from the overview page (specific)
    Given I am logged in as a user with the "View Bundle Lists,Delete Vehicle Bundles" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "delete"

  @entity
  Scenario: Users without the right permission can not access the entity's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the text "Car"
    And I should not see the link "Car"

  @entity
  Scenario Outline: Users with the right permission can access the entity's overview page (global)
    Given I am logged in as a user with the "View Bundle Lists,View Entity Lists" permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples: 
      | path                                   | link  |
      | "/admin/structure/entity-type/vehicle" | "Car" |
      | "/admin/structure/entity-type/animal"  | "Dog" |

  @entity
  Scenario: Users with the right permission can access the entity's overview page (specific)
    Given I am logged in as a user with the "View Bundle Lists,View List of Vehicle Car Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Car"
    When I click "Car"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "Dog"
    But I should see the text "Dog"

  @entity
  Scenario: Users without the right permission can not add entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "Add Car"

  @entity
  Scenario Outline: Users with the right permission can add entities from the overview page (global)
    Given I am logged in as a user with the "View Entity Lists,Add Entities" permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples: 
      | path                                       | link      |
      | "/admin/structure/entity-type/vehicle/car" | "Add Car" |
      | "/admin/structure/entity-type/animal/dog"  | "Add Dog" |

  @entity
  Scenario: Users with the right permission can add entities from the overview page (specific)
    Given I am logged in as a user with the "View Entity Lists,Add Vehicle Car Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "Add Car"
    When I click "Add Car"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "Add Dog"

  @entity
  Scenario: Users without the right permission can not delete entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "delete"

  @entity
  Scenario Outline: Users with the right permission can delete entities from the overview page (global)
    Given I am logged in as a user with the "View Entity Lists,Delete Any Entity" permissions
    And I visit <path>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples: 
      | path                                       |
      | "/admin/structure/entity-type/vehicle/car" |
      | "/admin/structure/entity-type/animal/dog"  |

  @entity
  Scenario: Users with the right permission can delete entities from the overview page (specific)
    Given I am logged in as a user with the "View Entity Lists,Delete Vehicle Car Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "delete"

  @cleanup
  Scenario Outline: This is a clean up step
    Given I am logged in as a user with the "administrator" role
    Given I visit "/admin/structure/entity-type"
    And I click <type_label>
    And I click "Delete"
    And I press the "Delete" button

    Examples: 
      | type_label |
      | "Vehicle"  |
      | "Animal"   |
