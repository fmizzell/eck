@api
Feature: Entity type management
  As a content architect
  I want to be able to create custom entity types
  so my content will be as lean and specific as possible.
  
  Background:
    Given I am logged in as a user with the "administrator" role
  
  Scenario: The entity type management page is completely functional
    Given I visit "/admin/structure/entity-type"
    Then I should see the heading "Entity types"
    Then I should see the link "Add entity type"
 
  @entity-type-create
  Scenario: I am able to create entity types
    Given I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "Entity Type" with "Test 12587"
    And I fill in "Machine-readable name" with "test_12587"
    And I press the "Save" button
    Then I should see the text "Entity type Test 12587 has been updated."
    
    Given I visit "/admin/structure/entity-type"
    Then I should see the text "Test 12587"

  @bundle-create
  Scenario: I am able to create bundles
    Given I visit "/admin/structure/entity-type/test_12587"
    And I click "Add bundle"
    And I fill in "Type" with "Bundle 19756"
    And I fill in "Machine-readable name" with "bundle_19756"
    And I press the "Save" button
    Then I should see the text "the bundle_19756 for entity type test_12587 has been saved"
    
    Given I visit "/admin/structure/entity-type/test_12587"
    Then I should see the text "Bundle 19756"

  @bundle-delete
  Scenario: I am able to delete bundles
    Given I visit "/admin/structure/entity-type/test_12587"
    And I click "Bundle 19756"
    And I click "Delete"
    And I press the "Delete" button
    Then I should see the text "The bundle 'bundle_19756' from the entity type 'test_12587' has been deleted"
    And I should not see "Bundle 19756"

  @entity-type-delete
  Scenario: I am able to delete entity types
    Given I visit "/admin/structure/entity-type"
    And I click "Test 12587"
    And I click "Delete"
    And I press the "Delete" button
    Then I should see the text "Entity type 'test_12587' has been deleted"
    And I should not see "Test 12587"
    
