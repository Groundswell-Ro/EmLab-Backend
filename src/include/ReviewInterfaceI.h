#pragma once
#include "AuthInterfaceI.h"
#include "../../../comunication/comm/ReviewInterface.h"

#include <Wt/Dbo/Session.h>


class ReviewInterfaceI : public Emlab::ReviewInterface
{
public:
	ReviewInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual int addReview(std::string userToken, Emlab::ReviewInfo reviewInfo, const Ice::Current &) override;
	virtual void delReview(std::string userToken, int reviewId, const Ice::Current &) override;

	virtual void setReviewTitle(std::string userToken, int reviewId, std::string title, const Ice::Current &) override;
	virtual void setReviewContent(std::string userToken, int reviewId, std::string content, const Ice::Current &) override;
	virtual void setReviewRating(std::string userToken, int reviewId, int rating, const Ice::Current &) override;
	
	virtual void addReviewPhoto(std::string userToken, int reviewId, Emlab::ImageData imageData, const Ice::Current &) override;
	virtual void delReviewPhoto(std::string userToken, int reviewId, int photoId, const Ice::Current &) override;

	virtual Emlab::SeqReviewInfo getSeqReviewInfo(std::string providerEmail, const Ice::Current &) override;

private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
