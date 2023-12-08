# include <nlbrctl/connector.hpp>

// on success return 0
// on error return some of nlbrctl::connector::* errors
// based on callback return value return error or 0
std::optional<int> nlbrctl::connector::open(const int domain, const int type, const int protocol, nlbrctl::connector::opt_cb_t callback) noexcept {
    if(this->socket__ = ::socket(domain, type, protocol); this->socket__ < 0) { // try to open connection
        return std::nullopt;
    }

    if(not callback.has_value()) {
        return 0; // ok
    }

    return callback.and_then([&](auto&& callback) noexcept -> std::optional<int> { // if success call callback function
        if(callback(this->socket__) < 0) {
            this->close(std::nullopt);

            return std::nullopt;
        }

        return 0; // ok
    });
}

// on success return 0
// on error return some of nlbrctl::connector::* errors
// based on callback return value return error or 0
std::optional<int> nlbrctl::connector::close(nlbrctl::connector::opt_cb_t callback) noexcept {
    if(not callback.has_value()) {
        if(::close(this->socket__) < 0) {
            return std::nullopt;
        }

        this->socket__ = -1;
        return 0;
    }

    return callback.and_then([&](auto&& callback) noexcept -> std::optional<int> { // if success call callback function
        const int ret = callback(this->socket__);

        if(ret < 0 and ::close(this->socket__) < 0) {
            return std::nullopt;
        }
        return 0; // ok
    });
}

void nlbrctl::connector::add_bridge(std::string name) noexcept {
    struct netilnk_request {
		struct nlmsghdr  n;
		struct ifinfomsg ifm;
		char             buf[512];
	} req;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = RTM_SETLINK;
	req.ifm.ifi_family = PF_BRIDGE;
}

void nlbrctl::connector::del_bridge(std::string name) noexcept {

}
