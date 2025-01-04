#include"FaithItem.hxx"

int ITK_user_main(int 	argc, char* argv[]) {
	printf("Utility started...\n");

	tag_t type = NULLTAG;
	tag_t create_input = NULLTAG;
	tag_t NewItem = NULLTAG;
	tag_t userTag = NULLTAG;
	tag_t Ufolder = NULLTAG;
	tag_t uom = NULLTAG;
	tag_t DStype = NULLTAG;
	tag_t DScreate_input = NULLTAG;
	tag_t NewDS = NULLTAG;


	//Get latest revision
	tag_t Item_rev = NULLTAG;


	//GRM Relation creation
	tag_t relation_type = NULLTAG;
	tag_t GRMrelation = NULLTAG;


	// variable
	char* item_id = NULL;
	char* item_name = NULL; 
	int status = ITK_ok;

	char* user = ITK_ask_cli_argument("-u=");
	char* password = ITK_ask_cli_argument("-p=");
	char* group = ITK_ask_cli_argument("-g=");

	status = ITK_init_module(user, password, group);
	if (status == ITK_ok) {
		printf("Login Successful.\n");
	}
	else {
		printf("Failed to login.\n");
	}

	//Item create
	status = TCTYPE_ask_type("A3BHMakePart", &type);		
	status = TCTYPE_construct_create_input(type, &create_input);
	status = AOM_set_value_string(create_input, "object_name", "Faith Item");
	status = AOM_set_value_string(create_input, "uom_tag", "Each");
	status = AOM_set_value_string(create_input, "a3MakeBuy", "Make");
	status = TCTYPE_create_object(create_input,&NewItem);
	status = AOM_save_with_extensions(NewItem);
	
	//Dataset create
	status = TCTYPE_find_type("PDF", "Dataset", &DStype);
	status = TCTYPE_construct_create_input(DStype, &DScreate_input);
	status = AOM_set_value_string(DScreate_input, "object_name", "Faith Dataset");
	status = TCTYPE_create_object(DScreate_input, &NewDS);
	status = AOM_save_with_extensions(NewDS);

	//get latest Rev
	status = ITEM_ask_latest_rev(NewItem, &Item_rev);

	// GRM relation
	status = GRM_find_relation_type("IMAN_specification", &relation_type);
	status = GRM_create_relation(Item_rev, NewDS,relation_type, NULLTAG, &GRMrelation);
	status = GRM_save_relation(GRMrelation);
	status = AOM_save_with_extensions(NewItem);

	//import name reference

	//status = AOM_lock(NewDS);
	status = AOM_refresh(NewDS, TRUE); //CHECKOUT THE DATASET
	status = AE_import_named_ref(NewDS, "PDF_Reference", "G:\\My Drive\\FaithPLM\\Online Batch Recorded Sessions\\4. Customization\\ITK_Programming\\NameRefTest.pdf", "", SS_BINARY);
	status = AOM_save_without_extensions(NewDS);
	status = AOM_refresh(NewDS, FALSE); // CHECKIN THE DATASET

	// place item to home folder
	status = SA_find_user2(user, &userTag);
	status = SA_ask_user_home_folder(userTag, &Ufolder);
	status = FL_insert(Ufolder, NewItem, 999);
	status = AOM_save_with_extensions(Ufolder);


	// printing newly created item	
	char* UOMvalue = NULL;
	status = ITEM_ask_id2(NewItem, &item_id);
	status = ITEM_ask_name2(NewItem, &item_name);
	status = ITEM_ask_unit_of_measure(NewItem, &uom);
	
	printf("\nItem ID\t\tItem Name\tUOM\n");
	printf("%s\t%s\t%d\n", item_id, item_name, uom);

	status = ITK_exit_module(true);
	if (status == ITK_ok) {
		printf("\n\nLogout Successful.\n");
	}
	else {
		printf("\nFailed to logout.\n");
	}

	(void)_getch();
	return status;
}