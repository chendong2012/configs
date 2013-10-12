ProgressDialog progressDialog;
progressDialog = new ProgressDialog(getActivity());
progressDialog.setIndeterminate(true);
progressDialog.setCancelable(true);
progressDialog.setTitle("Connecting");
progressDialog.setMessage("Attempting to connect...");
progressDialog.show();

progressDialog.setOnCancelListener(new OnCancelListener()
{

	@Override
	public void onCancel(DialogInterface dialog) 
	{
		showToast("ddddddddddddddddddd");
		//cancel(true);

	}

});
