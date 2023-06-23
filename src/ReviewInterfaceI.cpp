#include "include/ReviewInterfaceI.h"

ReviewInterfaceI::ReviewInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: authInterface_(authInterface)
{
	session_.setConnection(std::move(conn));
	session_.mapClass<User>("user");
	session_.mapClass<UserRole>("user_role");

	session_.mapClass<Profile>("profile");
	session_.mapClass<ProfileService>("profile_service");
	session_.mapClass<ProfileGalery>("profile_galery");
	session_.mapClass<ServiceAgeGroup>("service_age_group");

	session_.mapClass<Event>("event");
	session_.mapClass<EventService>("event_service");
	

	session_.mapClass<Review>("review");
	session_.mapClass<ReviewGalery>("review_galery");

}

int ReviewInterfaceI::addReview(std::string userToken, ReviewInfo reviewInfo, const Ice::Current &)
{
	int id = 0;
	std::cout << "\n\n addReview \n\n";
	return id;
}

void ReviewInterfaceI::delReview(std::string userToken, int reviewId, const Ice::Current &)
{
	std::cout << "\n\n delReview \n\n";
}


void ReviewInterfaceI::setReviewTitle(std::string userToken, int reviewId, std::string title, const Ice::Current &)
{
	std::cout << "\n\n setReviewTitle \n\n";
}

void ReviewInterfaceI::setReviewContent(std::string userToken, int reviewId, std::string content, const Ice::Current &)
{
	std::cout << "\n\n setReviewContent \n\n";
}

void ReviewInterfaceI::setReviewRating(std::string userToken, int reviewId, int rating, const Ice::Current &)
{
	std::cout << "\n\n setReviewRating \n\n";
}

	
void ReviewInterfaceI::addReviewPhoto(std::string userToken, int reviewId, ImageData imageData, const Ice::Current &)
{
	std::cout << "\n\n  \n\n";
}

void ReviewInterfaceI::delReviewPhoto(std::string userToken, int reviewId, int photoId, const Ice::Current &)
{
	std::cout << "\n\n addReviewPhoto \n\n";
}


SeqReviewInfo ReviewInterfaceI::getSeqReviewInfo(std::string providerEmail, const Ice::Current &)
{
	SeqReviewInfo seqReviewInfo;
	std::cout << "\n\n getSeqReviewInfo \n\n";
	return seqReviewInfo;
}